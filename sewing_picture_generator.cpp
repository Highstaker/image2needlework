#include "headers/TGA_processor.h"
#include <pthread.h>

void* renderOutput(void *lp);

using namespace std;

////////////////////
//////PARAMS///////
//////////////////

int line_width = 1;
int cell_size = 10;
short thread_amount = 1;

//////////////////////
////GLOBALS//////////
/////////////////
//all these were previously defined in main, but since multithreading support,
//preliminary global definition is required
string filename ;
string out_filename;
int out_width;
int out_height;
int e;
int n_cells_x;
int n_cells_y;
Colour_24 c;
TGAImage *in_image;
TGAImage *out_image;

int main(int argc, char **argv)
{
	filename ;
	out_filename = "out_needlework.tga";
	
	for(int i=0;i<argc ;i++)//reading options
	{
		
		if((string)argv[i] == "-i")
		{
			filename = argv[i+1];
			//cerr << "filename "<< filename << endl; return 1;//debug
		}

		if((string)argv[i] == "-o")
		{
			out_filename = argv[i+1];
			//cerr << "filename "<< filename << endl; return 1;//debug
		}
		
		if((string)argv[i] == "--cell-size")
		{
			cell_size = atoi(argv[i + 1]);
		}

		if((string)argv[i] == "--border-thickness")
		{
			line_width = atoi(argv[i + 1]);
		}

		if((string)argv[i] == "--render-threads")
		{
			thread_amount = atoi(argv[i + 1]);
			if((thread_amount < 1) || (thread_amount > 4) || (thread_amount == 3))
				{cerr << "Please set --render-threads to 1,2 or 4" << endl; return 1;}
		}

	}//end options reading

	in_image = new TGAImage(filename);

	n_cells_x = in_image -> m_width;
	n_cells_y = in_image -> m_height;

	cerr << "Width: " << in_image -> m_width  << "\t" << "Height: " << in_image -> m_height << endl;//debug

	if(in_image -> image_type != IMAGE_PALETTE)
	{
		cerr << "Image type must be indexed; image type: " << (int)(in_image -> image_type) << endl;
		return 1;
	}

	e = line_width + cell_size;
	out_width = line_width + n_cells_x * e;
	out_height = line_width + n_cells_y * e;

	//Colour_24 c;
	c.a = 255;
	out_image = new TGAImage(out_width,out_height,IMAGE_TRUECOLOR);

	pthread_t thread_id[thread_amount];



	for(int i=0; i<thread_amount ; i++)
	{
		//int thread_param = thread_amount *100 + i;
		pthread_create(&thread_id[i],0,&renderOutput,(void*)i);
	cerr << "Mile1" << endl;//debug	
	}

	for(int i=0; i<thread_amount ; i++)
	{
		pthread_join(thread_id[i],NULL);
	}

//that is sent to separate thread
/*
	for(int i = 0 ; i< out_width ; i++)
		for(int j = 0; j < out_height ; j++)
		{
			if(i%e<line_width)
		{//draw gridline
			c.r = 0; c.g = 0; c.b = 0;
			out_image -> setPixel(c,i,j);
		}//if
		else if(j%e<line_width)
		{//draw gridline
			c.r = 0; c.g = 0; c.b = 0;
			out_image -> setPixel(c,i,j);
		}//if
		else
		{			
			c = in_image -> getPixel(i/e,j/e)	;
			out_image -> setPixel(c,i,j);
		}//else
			//cerr << i << "\t" << j << "\t" << (int)c.r << "\t" << (int)c.g << "\t" << (int)c.b << endl;//debug
	}//for
	//cerr << "Mile1" << endl;//debug
	//c.r = 0; c.g = 255; c.b = 0; out_image -> drawSquare(c,0,0,4000,4000);//test

	
	 //symbol drawing sequence
	cFreetype symbols( "/usr/share/fonts/truetype/freefont/FreeSans.ttf",cell_size) ;
	//symbols.get_char_pixel_data(65);

	
	//cerr << "Mile2" << endl;//debug

	for(int k = 0; k< n_cells_x; k++)
		for(int n = 0; n< n_cells_y; n++)
		{
			symbols.get_char_pixel_data(65 + (in_image -> getPalettePixelIndex(k, (in_image->m_height ) - 1 - n)));
			c = in_image -> getPixel(k, (in_image->m_height ) - 1 - n);
			//int lightness = 255;
			int lightness = 0.3 * c.r + 0.59 * c.g + 0.11 * c.b;
//int lightness = c.max_channel_value()/2 + c.min_channel_value()/2;
//cerr << "c.r: " << c.r << "\t c.max_channel_value: " << c.max_channel_value() << "\t c.min_channel_value: " << c.min_channel_value() <<  "\t lightness: " << lightness << endl; //debug
			if(lightness > 122)
			{
				c.r = 0; c.b = 0; c.g = 0;
			}
			else 
			{
				c.r = 255; c.g = 255; c.b = 255;
			}

			for(int i=0;i<symbols.rows;i++)
				for(int j=0;j<symbols.width;j++)
				{
					unsigned char gray = symbols.buffer[i*symbols.width+j];
					c.a = gray ;					
					int x1 = line_width + e*k + j ; if(x1<0)x1=0; if(x1>=out_width)x1=(out_width-1);
					int y1 = out_height - line_width - e * n - i ; if(y1<0)y1=0; if(y1>=out_height)x1=(out_height-1);
					out_image -> blendPixel(c,x1, y1,BLEND_MODE_NORMAL);
					//Colour_24 c_test = out_image -> getPixel(k,n); cerr << (int)c_test.a << endl;//debug
				}

			}
			*/


			out_image -> WriteImage(out_filename);

			return 0;

}//main

void* renderOutput(void *lp)
{
cerr << "tMile1" << endl;//debug

int param = (int)lp;
cerr << "param: " << param << endl;//debug
int startx = 0;
int endx = out_width;
int starty = 0;
int endy =out_height;


if(thread_amount == 1)
{
startx = 0;
endx = out_width;
starty = 0;
endy =out_height;
}

if(thread_amount == 2)
{
startx = 0 + param * out_width/2;
endx = out_width/2 + param * out_width/2;
starty = 0;
endy = out_height;
//if(param == 0){return 0;};//debug
}

if(thread_amount == 4)
{
startx = param/2 * out_width/2;
endx = param/2 * out_width/2 + out_width/2;
starty = param%2 * out_height/2;
endy = param%2 *out_height/2 + out_height/2;
}

for(int i = startx ; i< endx ; i++)
	for(int j = starty; j < endy ; j++)
	{
			//cerr << "tMile1.5" << endl;//debug
		if(i%e<line_width)
		{//draw gridline
			c.r = 0; c.g = 0; c.b = 0;
		//	cerr << "tMile1.6" << endl;//debug
			out_image -> setPixel(c,i,j);
		//	cerr << "tMile1.7" << endl;//debug
		}//if
		else if(j%e<line_width)
		{//draw gridline
			c.r = 0; c.g = 0; c.b = 0;
			out_image -> setPixel(c,i,j);
		}//if
		else
		{			
			c = in_image -> getPixel(i/e,j/e)	;
			out_image -> setPixel(c,i,j);
		}//else
			//cerr << i << "\t" << j << "\t" << (int)c.r << "\t" << (int)c.g << "\t" << (int)c.b << endl;//debug
	}//for
	//cerr << "Mile1" << endl;//debug
	//c.r = 0; c.g = 255; c.b = 0; out_image -> drawSquare(c,0,0,4000,4000);//test
cerr << "tMile2" << endl;//debug

	 //symbol drawing sequence
cFreetype symbols( "/usr/share/fonts/truetype/freefont/FreeSans.ttf",cell_size) ;
	//symbols.get_char_pixel_data(65);


	//cerr << "Mile2" << endl;//debug

if(thread_amount == 1)
{
startx = 0;
endx = n_cells_x;
starty = 0;
endy =n_cells_y;
}

if(thread_amount == 2)
{
startx = 0 + param * n_cells_x/2;
endx = n_cells_x/2 + param * n_cells_x/2;
starty = 0;
endy = n_cells_y;
//if(param == 0){return 0;};//debug
}

if(thread_amount == 4)
{
startx = param/2 * n_cells_x/2;
endx = param/2 * n_cells_x/2 + n_cells_x/2;
starty = param%2 * n_cells_y/2;
endy = param%2 *n_cells_y/2 + n_cells_y/2;
}

	for(int k = startx; k< endx; k++)
		for(int n = starty; n< endy; n++)
		{
			symbols.get_char_pixel_data(65 + (in_image -> getPalettePixelIndex(k, (in_image->m_height ) - 1 - n)));
			c = in_image -> getPixel(k, (in_image->m_height ) - 1 - n);
			//int lightness = 255;
			int lightness = 0.3 * c.r + 0.59 * c.g + 0.11 * c.b;
//int lightness = c.max_channel_value()/2 + c.min_channel_value()/2;
//cerr << "c.r: " << c.r << "\t c.max_channel_value: " << c.max_channel_value() << "\t c.min_channel_value: " << c.min_channel_value() <<  "\t lightness: " << lightness << endl; //debug
			if(lightness > 122)
			{
				c.r = 0; c.b = 0; c.g = 0;
			}
			else 
			{
				c.r = 255; c.g = 255; c.b = 255;
			}

			for(int i=0;i<symbols.rows;i++)
				for(int j=0;j<symbols.width;j++)
				{
					unsigned char gray = symbols.buffer[i*symbols.width+j];
					c.a = gray ;					
					int x1 = line_width + e*k + j ; if(x1<0)x1=0; if(x1>=out_width)x1=(out_width-1);
					int y1 = out_height - line_width - e * n - i ; if(y1<0)y1=0; if(y1>=out_height)x1=(out_height-1);
					out_image -> blendPixel(c,x1, y1,BLEND_MODE_NORMAL);
					//Colour_24 c_test = out_image -> getPixel(k,n); cerr << (int)c_test.a << endl;//debug
				}
		}


cerr << "thread lp: " << (int)lp << endl;//debug
return 0;
}