#include "headers/TGA_processor.h"

using namespace std;

////////////////////
//////PARAMS///////
//////////////////

int line_width = 1;
int cell_size = 10;
short thread_amount = 1;


int main(int argc, char **argv)
{
	string filename ;
string out_filename = "out_needlework.tga";
	
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
		}

	}//end options reading

	TGAImage *in_image = new TGAImage(filename);

	int n_cells_x = in_image -> m_width;
	int n_cells_y = in_image -> m_height;

	cerr << "Width: " << in_image -> m_width  << "\t" << "Height: " << in_image -> m_height << endl;//debug

	if(in_image -> image_type != IMAGE_PALETTE)
	{
		cerr << "Image type must be indexed; image type: " << (int)(in_image -> image_type) << endl;
		return 1;
	}

	int e = line_width + cell_size;
	int out_width = line_width + n_cells_x * e;
	int out_height = line_width + n_cells_y * e;

	Colour_24 c; c.a = 255;
	TGAImage *out_image = new TGAImage(out_width,out_height,IMAGE_TRUECOLOR);


	//cerr << "array_size " << out_width*out_height << endl;//debug

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
	cerr << "Mile1" << endl;//debug
	//c.r = 0; c.g = 255; c.b = 0; out_image -> drawSquare(c,0,0,4000,4000);//test

	
	 //symbol drawing sequence
	cFreetype symbols( "/usr/share/fonts/truetype/freefont/FreeSans.ttf",cell_size) ;
	//symbols.get_char_pixel_data(65);

	
	cerr << "Mile2" << endl;//debug

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
			

	/*
	//testing. drawing a square in each cell
	 int symbol_size = 1; 
	 int e2 =line_width + cell_size/2;
	 for(int i=0;i<n_cells_x;i++)
	 for(int j=0;j<n_cells_y;j++)
	 {
		 out_image -> drawSquare(c,e*i + e2 - symbol_size,e*j + e2 - symbol_size,e*i + e2 + symbol_size,e*j + e2 + symbol_size);
		
}
*/

out_image -> WriteImage(out_filename);

return 0;

}//main
