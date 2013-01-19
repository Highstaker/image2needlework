#ifndef __TGA_PROCESSOR__
#define __TGA_PROCESSOR__

//includes
#ifndef __VECTOR__
#define __VECTOR__
#include <vector>
#endif

#ifndef __MATH__
#define __MATH__
#include <math.h>
#endif

#ifndef __STRING__
#define __STRING__
#include <string>
#endif

#ifndef __FSTREAM__
#define __FSTREAM__
#include <fstream>
#endif

#ifndef __IOSTREAM__
#define __IOSTREAM__
#include <iostream>
#endif

#ifndef __CK_FREETYPE_H__
#define __CK_FREETYPE_H__
#include "ck_freetype.h"
#endif

#ifndef __COLORS_H__
#define __COLORS_H__
#include "Colors.h"
#endif

using namespace std;


//DEFINITIONS

//blendPixel() blending modes
#define BLEND_MODE_NORMAL 0


//Image types
#define IMAGE_PALETTE 1
#define IMAGE_TRUECOLOR 2
#define IMAGE_MONO 3
#define IMAGE_PALETTE_RLE 9
#define IMAGE_TRUECOLOR_RLE 10
#define IMAGE_MONO_RLE 11


//Structure with image data
struct TGAImage 
{

	unsigned short m_height;//Height of image
	unsigned short m_width;//Width of image
	unsigned char identifier_length,palette_type,image_type,byteCount;
	int palette_elements;



	//Constructor
	TGAImage();

	//Destructor
	~TGAImage(){ delete[] m_pixels; m_pixels = 0; }

	//Overridden Constructor
	TGAImage(short width, short height, short type);

	//From-file constructor
	TGAImage(string filename);

	//Set all pixels at once
	void setAllPixels(Colour_24 pixels);

	//set individual pixels
	void setPixel(Colour_24 inputcolor, int xposition, int yposition);

	//blend the specified pixel with the one present at this position
	void blendPixel(Colour_24 inputcolor, int xposition, int yposition, int blend_mode);

	//draw a square of certain color between point (x1,y1) and point (x2,y2)
	void drawSquare(Colour_24 c, int x1, int y1, int x2, int y2);

	//get pixel defined by coordinates
	Colour_24 getPixel(int x, int y);

	//for indexed images, get the pixel index in palette data for specified pixel
	int getPalettePixelIndex(int x, int y);

	//Image output sequence
	void WriteImage(string filename);

	//General getters and setters

	//void setWidth(short width);
	//void setHeight(short height);

	//Convert indexed image to truetype
	void type_palette2Truetype();


private:

		//Image input sequence
	bool ReadImage(string filename);

		//store the pixels
	Colour_24 *m_pixels;

	Colour_24 *palette_data;
	unsigned char *palette_pixels;

		//convert 2D to 1D indexing
	int convert2dto1d(int x, int y); 



};

//Default Constructor
TGAImage::TGAImage() 
{

}

//Overridden Constructor
TGAImage::TGAImage(short width, short height, short type) 
{
	image_type = type;
	m_width = width;
	m_height = height;
	m_pixels = new Colour_24[m_width*m_height];	
}

//From-file constructor
TGAImage::TGAImage(string filename)
{
	//oneD_conversion = conversion_method;
	if(!ReadImage(filename))
	{
		cerr << "Could not load image" << endl;
	}
}

//Set all pixels at once
void TGAImage::setAllPixels(Colour_24 pixels)
{
	for(int i=0; i<m_width*m_height; i++)
	{
		m_pixels[i].r = pixels.r;
		m_pixels[i].g = pixels.g;
		m_pixels[i].b = pixels.b;
		m_pixels[i].a = pixels.a;
	}
}

//Set indivdual pixels
void TGAImage::setPixel(Colour_24 inputcolor, int x, int y) 
{
	m_pixels[convert2dto1d(x,y)] = inputcolor;		
}

//blend the specified pixel with the one present at this position
void TGAImage::blendPixel(Colour_24 inputcolor, int x, int y, int blend_mode = BLEND_MODE_NORMAL)
{

	fColour_24 blendedcolorf;
	fColour_24 inputcolorf;
	Colour_24 presentcolor = getPixel(x,y);
	fColour_24 presentcolorf;

	inputcolorf =  Colour256_to_fColour( inputcolor);
	presentcolorf =  Colour256_to_fColour( presentcolor);

//cerr << (int)presentcolor.a << endl ;//debug

	switch(blend_mode)
	{
		case BLEND_MODE_NORMAL:
		{	
			blendedcolorf.a = presentcolorf.a * (1 - inputcolorf.a) + inputcolorf.a;
			blendedcolorf.r = (presentcolorf.r * presentcolorf.a * (1 - inputcolorf.a) + inputcolorf.r * inputcolorf.a)/ (blendedcolorf.a+0.00001) ;
			blendedcolorf.g = (presentcolorf.g * presentcolorf.a * (1 - inputcolorf.a ) + inputcolorf.g * inputcolorf.a)/ (blendedcolorf.a+0.00001) ;
			blendedcolorf.b = (presentcolorf.b * presentcolorf.a * (1 - inputcolorf.a) + inputcolorf.b * inputcolorf.a)/ (blendedcolorf.a+0.00001) ;

			break;
		}

		default:break;

	}//switch


	Colour_24 blendedcolor;
	blendedcolor = fColour_to_Colour256( blendedcolorf);

	m_pixels[convert2dto1d(x,y)] = blendedcolor;	


	

}//blendPixel

//Convert 2d array indexing to 1d indexing
int TGAImage::convert2dto1d(int x, int y)
{		
	return  x + m_width * y;
}

Colour_24 TGAImage::getPixel(int x, int y)
{	
	Colour_24 a;

	if(image_type == IMAGE_TRUECOLOR)
	{
		a = m_pixels[convert2dto1d(x,y)];
		if(byteCount == 3){a.a = 255;}
	}

	if(image_type == IMAGE_PALETTE)
	{
		int index = palette_pixels[convert2dto1d(x,y)];
		a = palette_data[index];
		if(byteCount == 3){a.a = 255;}
		
	}

	return a;

}

//Image output sequence
void TGAImage::WriteImage(string filename) {

	if(image_type == IMAGE_PALETTE)
		type_palette2Truetype();

	//Error checking
	if (m_width <= 0 || m_height <= 0)
	{
		cerr << "Error! Negative size!" << endl;
		return;
	}

	ofstream o(filename.c_str(), ios::out | ios::binary);

	//Header
	o << (char)0  //Zero identifier length
		<< (char)0 // 0 - No color map; 1 - Color map present
		<< (char)2 // 0 - Nothing ; 1 - Indexed ; 2 -TrueColor (24-bit) ; 3 - Monochrome; 9,10,11 - same as 1,2,3 but with RLE compression                        
		<< (char)0 << (char)0 //Index of palette's first element
		<< (char)0 << (char)0 //Amount of elements in palette 
		<< (char)0 //Bits per palette's element (usually 15,16,24,32)
		<< (char)0 << (char)0  //Absolute location of bottom-left corner by x(horizontal) axis
		<< (char)0 << (char)0  //Absolute location of bottom-left corner by y(vertical) axis
		<< (char)(m_width % 256) <<  (char)(m_width / 256) //Info on image width
		<< (char)(m_height % 256) <<  (char)(m_height / 256) //Info on image height
		<< (char)32 //Bits per pixel                        
		<< (char)0;//Some descriptor, no idea.

	//Pixel data
		for (int i=0;i<m_height*m_width;i++) 
		{
		o << m_pixels[i].b //Blue channel
			<< m_pixels[i].g //Green channel
			<< m_pixels[i].r //Red Channel
			<< m_pixels[i].a //Alpha channel
			;}   

	//close the file
			o.close();

		}

//Image input sequence
		bool TGAImage::ReadImage(string filename)
		{

			char header[20];


			ifstream file(filename.c_str(), ios::in | ios::binary);

			file.read(header,17);

	for(int i=0;i<17;i++){cerr << (int)header[i] << endl;}//debug
		cerr << "////////////" << endl;//debug

	identifier_length = header[0];
	palette_type = header[1];
	image_type = header[2];

	if(header[12] < 0)//avoiding a glitch in certain files
	{
		m_width = 256 * (header[13]+1) + header[12];
	}
	else
	{
		m_width = header[12] + 256 * header[13];
	}

if(header[14] < 0)//avoiding a glitch in certain files
{
	m_height =  256 * (header[15]+1) + header[14] ;
}
else
{
	m_height = header[14] + 256 * header[15];
}

	//errors
if((m_width <= 0) || (m_height <= 0))
{
	cerr << "Error: wrong size" << endl;
	file.close();
	return false;
}

if(image_type == IMAGE_TRUECOLOR)
{
	byteCount = header[16]/8;
	long imageSize = m_height * m_width * byteCount ;
	char data[imageSize+1];
	file.seekg(18);
	file.read(data,imageSize);
	if(byteCount == 3)
	{
		m_pixels = new Colour_24[imageSize + imageSize/3];
	}

	if(byteCount == 4)
	{
		m_pixels = new Colour_24[imageSize];
	}

	for(int i = 0; i < imageSize; i++)
	{

		if(byteCount == 3)
		{
			switch(i%3)
			{
				case 0: 
				{
					m_pixels[i/3].b = data[i];
				}

				case 1: 
				{
					m_pixels[i/3].g = data[i];
				}

				case 2: 
				{
					m_pixels[i/3].r = data[i];
					m_pixels[i/3].a = 255;
				}

				default : break;
			}//switch
		}

		if(byteCount == 4)
		{
			switch(i%4)
			{
				case 0: 
				{
					m_pixels[i/4].b = data[i];
				}

				case 1: 
				{
					m_pixels[i/4].g = data[i];
				}

				case 2: 
				{
					m_pixels[i/4].r = data[i];
				}

				case 3: 
				{
					m_pixels[i/4].a = data[i];
				}

				default : break;
			}//switch
		}

		
	}//for

}

else if(image_type == IMAGE_PALETTE)
{ 
	byteCount = header[7]/8; 
	palette_elements = header[5]; //needs fixing, what's with [6]
	long imageSize = m_height * m_width ;
	long palette_size = palette_elements*byteCount;
	unsigned char palette_temp[ palette_size ];
	file.seekg(18);
	file.read( (char*)palette_temp , palette_size );

	palette_data = new Colour_24[palette_size];

	for(int i=0;i<=palette_size;i++)
	{

		if(byteCount == 3)
		{
			switch(i%3)
			{
				case 0: 
				{
					palette_data[i/3].b = palette_temp[i];
				}

				case 1: 
				{
					palette_data[i/3].g = palette_temp[i];
				}

				case 2: 
				{
					palette_data[i/3].r = palette_temp[i];
					palette_data[i/3].a = 255;
				}

				default : break;
			}//switch
		}

		if(byteCount == 4)
		{
			switch(i%4)
			{
				case 0: 
				{
					palette_data[i/4].b = palette_temp[i];
				}

				case 1: 
				{
					palette_data[i/4].g = palette_temp[i];
				}

				case 2: 
				{
					palette_data[i/4].r = palette_temp[i];
				}

				case 3: 
				{
					palette_data[i/4].a = palette_temp[i];
				}

				default : break;
			}//switch
		}

		


}//for

file.seekg(18+palette_size);

palette_pixels = new unsigned char[imageSize];

file.read((char*)palette_pixels,imageSize);

for(int i=0; i<9; i++) {cerr << (int)palette_data[i].r << "\t" << (int)palette_data[i].g << "\t" << (int)palette_data[i].b<< "\t" << (int)palette_data[i].a<<endl;}//debug
cerr << "//////////////////" << endl;//debug
for(int i=0; i<9; i++) {cerr << (int)palette_pixels[i]<<endl;}//debug
cerr << "//////////////////" << endl;//debug

}


else
{
	cerr << "Unsupported image type: " << (int)image_type << endl;
	file.close();
	return false;
}

file.close();

return true;

}//ReadImage

void TGAImage::drawSquare(Colour_24 c, int x1, int y1, int x2, int y2)
{
	for(int i=x1;i<=x2;i++)
		for(int j=y1;j<=y2;j++)
		{
			setPixel(c,i,j);
		}

}//drawSquare


#endif

void TGAImage::type_palette2Truetype()
{
	image_type = IMAGE_TRUECOLOR;
	unsigned long imageSize = m_width * m_height * byteCount;
	//Colour_24 temp[imageSize];
	m_pixels = new Colour_24[imageSize];


	for(int i=0; i< imageSize;i++)
	{
		unsigned char a = palette_pixels[i];
		m_pixels[i] = palette_data[a];
	}

	

	delete[] palette_pixels;
	delete[] palette_data;

}

int TGAImage::getPalettePixelIndex(int x, int y)
{
	int index = palette_pixels[convert2dto1d(x,y)];
	return index;
}