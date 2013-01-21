#include "headers/TGA_processor.h"
#include <pthread.h>
#include <sys/time.h>

void* renderOutput(void *lp);

using namespace std;

////////////////////
//////PARAMS///////
//////////////////

int line_width = 1;
int cell_size = 10;
short thread_amount = 1;
const Colour_24 null_color = {0,0,0,0};

//////////////////////
////GLOBALS//////////
/////////////////
//many of these were previously defined in main, but since multithreading support,
//preliminary global definition is required
string filename ;
string out_filename;
int out_width;
int out_height;
int e;
int n_cells_x;
int n_cells_y;
//Colour_24 cg;
TGAImage *in_image;
TGAImage *out_image;
//ofstream *o;
pthread_mutex_t the_mutex;



int main(int argc, char **argv)
{

	time_t start_time, end_time; long elapsed_time;
	time(&start_time);
	
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
	//c.a = 255;

	out_image = new TGAImage(out_width,out_height,IMAGE_TRUECOLOR);
	out_image -> setAllPixels(null_color);

	pthread_t thread_id[thread_amount];

	pthread_mutex_init(&the_mutex,NULL);

//o = new ofstream(filename.c_str(), ios::out | ios::binary);

	for(int i=0; i<thread_amount ; i++)
	{
		//int thread_param = thread_amount *100 + i;
		pthread_create(&thread_id[i],0,&renderOutput,(void*)i);
	//cerr << "Mile1" << endl;//debug	
}

for(int i=0; i<thread_amount ; i++)
{
	pthread_join(thread_id[i],NULL);
}

//o -> close();

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

			time(&end_time);
			elapsed_time = end_time - start_time;
			cout << "Elapsed  time : " << elapsed_time << " seconds." << endl;

			return 0;

}//main

void* renderOutput(void *lp)
{
cerr << "thread start lp: " << (int)lp << endl;//debug

Colour_24 c;c.a = 255;
int param = (int)lp;
cerr << "param: " << param << endl;//debug
int startx = 0;
int endx = out_width;
int starty = 0;
int endy =out_height;

TGAImage *local_pixels = new TGAImage(out_width,out_height/thread_amount,IMAGE_TRUECOLOR);

//if(thread_amount == 1)
//{
startx = 0;
endx =  out_width;
starty = 0;
endy = out_height/thread_amount;
//startx = 0;
//endx = local_pixels -> m_width;
//starty = 0;
//endy = local_pixels -> m_height;
//}
cerr << "mile0.1" <<endl;//debug
cerr << param << " startx: " << startx << endl;//debug
cerr << param << " endx: " << endx << endl;//debug
cerr << param << " starty: " << starty << endl;//debug
cerr << param << " endy: " << endy << endl;//debug
/*
if(thread_amount == 2)
{
startx = 0 + param * out_width/2;
endx = out_width/2 + param * out_width/2;
starty = 0;
endy = out_height;
if(param == 0){return 0;};//debug
}

if(thread_amount == 4)
{
startx = param/2 * out_width/2;
endx = param/2 * out_width/2 + out_width/2;
starty = 0;
endy = out_height
}
*/

for(int i = startx ; i< endx ; i++)
	for(int j = starty; j < endy ; j++)
	{
		//(local_pixels -> m_pixels[0]) = c;
		//cerr << i << '\t' << j << '\t' << sizeof(local_pixels) << endl;//debug
		//Colour_24 temp_test = (local_pixels -> getPixel(i,j));
		//if(param){cerr << i << '\t' << j << '\t' << (int)c.r << '\t' << (int)c.g << '\t' << (int)c.b << '\t' << (int)c.a << '\t' << endl;}//debug
		if(  //(temp_test.r == null_color.r) && (temp_test.g == null_color.g) &&(temp_test.b == null_color.b) &&(temp_test.a == null_color.a) &&
			true)
		{
			//cerr << "tMile1.5" << endl;//debug
			if(i%e<line_width)
		{//draw gridline
			c.r = 0; c.g = 0; c.b = 0;
		//	cerr << "tMile1.6" << endl;//debug
			//pthread_mutex_lock(&the_mutex);
			local_pixels -> setPixel(c,i,j);

			//pthread_mutex_unlock(&the_mutex);
		//	cerr << "tMile1.7" << endl;//debug
		}//if
		else if(j%e<line_width)
		{//draw gridline
			c.r = 0; c.g = 0; c.b = 0;
			//pthread_mutex_lock(&the_mutex);
			local_pixels -> setPixel(c,i,j);
			//pthread_mutex_unlock(&the_mutex);
		}//if
		else
		{			
			c = in_image -> getPixel(i/e,(j + param * (local_pixels -> m_height))/e);
			//pthread_mutex_lock(&the_mutex);
			local_pixels -> setPixel(c,i,j);
			//pthread_mutex_unlock(&the_mutex);
		}//else
			//cerr << i << "\t" << j << "\t" << (int)c.r << "\t" << (int)c.g << "\t" << (int)c.b << endl;//debug
	}
		//else{cerr<<"test_else"<<endl;}

	}//for
	//cerr << "Mile1" << endl;//debug
	//c.r = 0; c.g = 255; c.b = 0; local_pixels -> drawSquare(c,0,0,4000,4000);//test
//cerr << "tMile2" << endl;//debug
cerr << "mile0.3" <<endl;//debug
	 //symbol drawing sequence
cFreetype symbols( "/usr/share/fonts/truetype/freefont/FreeSans.ttf",cell_size) ;
	//symbols.get_char_pixel_data(65);


	//cerr << "Mile2" << endl;//debug

//if(thread_amount == 1)
//{
// startx = 0 +  (float)param/(float)thread_amount * n_cells_x;
// endx = (float)(param+1)/(float)thread_amount * n_cells_x;
// starty = 0;
// endy =n_cells_y;
startx = 0 ;
endx = n_cells_x;
starty = 0;
endy =n_cells_y / thread_amount;
//}


/*
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
*/

//if(thread_amount == 1)//temporary, until I figure out how to make itwork with multithreading
{
for(int k = startx; k< endx; k++)
	for(int n = starty; n< endy; n++)
	{
		symbols.get_char_pixel_data(65 + (in_image -> getPalettePixelIndex(k, (in_image->m_height ) - 1 - n)));
		//cerr << k << '\t' << n << endl;//debug
		c = in_image -> getPixel(k, (in_image->m_height ) - 1 - n);
//if(!param)cerr << param << '\t' << (int)c.r <<endl;else return 0;//debug
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
				//cerr << param << '\t'<<  k << '\t' << n << '\t' << i << '\t' << j << endl;//debug
				unsigned char gray = symbols.buffer[i*symbols.width+j];
				c.a = gray ;					
				int x1 = line_width + e*k + j + e/4; 
				if( (x1<0) || (x1>=(local_pixels -> m_width)) )continue;
				int y1 = out_height - line_width - e * n - i; 
				if( (y1<0) || (y1>=(local_pixels -> m_height)) )continue;
				local_pixels -> blendPixel(c,x1, y1,BLEND_MODE_NORMAL);
					//Colour_24 c_test = out_image -> getPixel(k,n); cerr << (int)c_test.a << endl;//debug
			}
		}

}

//if(param){return 0;}//debug
//local_pixels -> WriteImage("test.tga");//debug

 cerr << "mile1" << endl;//debug
//long size_of_local_chunk = out_width * out_height / thread_amount ;
 long size_of_local_chunk = (local_pixels -> m_width) * (local_pixels -> m_height);

pthread_mutex_lock(&the_mutex);
memcpy((void*)&(out_image -> m_pixels[size_of_local_chunk*param]),(const void*)(local_pixels -> m_pixels),size_of_local_chunk * sizeof(unsigned char) * 4 ) ;
pthread_mutex_unlock(&the_mutex);

cerr << "mile1.1" << endl;//debug

delete[] (local_pixels -> m_pixels);
//delete local_pixels;

 cerr << "mile1.2" << endl;//debug

/*
startx =0 + (float)param/(float)thread_amount * (out_image -> m_height) * (out_image -> m_width);
endx = (float)(param+1)/(float)thread_amount * (out_image -> m_height) * (out_image -> m_width);

		cerr << param << " startx: " << startx << endl;//debug
		cerr << param << " endx: " << endx << endl;//debug



		out_image -> WriteImagePartial(o,
			startx,
			endx,
			!param);
*/


cerr << "thread end lp: " << (int)lp << endl;//debug
return 0;
}