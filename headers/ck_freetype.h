#ifndef __IOSTREAM__
#define __IOSTREAM__
#include <iostream>
#endif

#ifndef __STRING__
#define __STRING__
#include <string>
#endif

#ifndef __FREETYPE_2__
#define __FREETYPE_2__
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

using namespace std;

class cFreetype
{
	FT_Library library;
	FT_Face face;



	public:
		int rows;
		int width ;
		unsigned char*  buffer ;

		//constructor
		cFreetype();

		//overridden constructor
		cFreetype(string font_path, int font_size); 		

		void get_char_pixel_data(int charcode);


};//cFreetype


cFreetype::cFreetype(string font_path, int font_size)
{

	int error = FT_Init_FreeType(&library);
	if(error)
	{
		cerr << "Could not initialize library. Error number: " << error << endl;
	} 

	error = FT_New_Face( library, font_path.c_str(), 0, &face );
	if(error)
	{
		cerr << "Could not assign face. Error number: " << error << endl;
	} 

	error = FT_Set_Char_Size( face, /* handle to face object */
	                         0, /* char_width in 1/64th of points */
	                         font_size*64, /* char_height in 1/64th of points */ 
	                         72, /* horizontal device resolution */ 
	                         72 );	/* vertical device resolution */

	if(error)
	{
		cerr << "Could not set character size. Error number: " << error << endl;
	} 

}//cFreetype

void cFreetype::get_char_pixel_data(int charcode)
{

	int error = FT_Load_Char( face, charcode, FT_LOAD_RENDER );
	if(error)
	{
		cerr << "Could not load character. Error number: " << error << endl;
	} 

	FT_GlyphSlot slot = face->glyph; /* a small shortcut */

	rows = slot->bitmap.rows;
	width = slot->bitmap.width;
	buffer = slot->bitmap.buffer;
	char pixel_mode = slot->bitmap.pixel_mode;

	//cerr << "rows " <<rows << "\t" << "width " << width << "\t" << "pixel_mode " << (int)pixel_mode << endl;//debug
	//int i;for(i=0; i<rows*width;i++){cerr << (int)buffer[i] << endl;}//debug

}//get_char_pixel_data
