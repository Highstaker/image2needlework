struct Colour_16
{
	unsigned char b,g,r;   //Can be 0 - 255

	int max_channel_value()
	{
		int m = 0;
		if(r > m){m = r;}
		if(g > m){m = g;}
		if(b > m){m = b;}
		return m;
	}

	int min_channel_value()
	{
		int m = 255;
		if(r < m){m = r;}
		if(g < m){m = g;}
		if(b < m){m = b;}
		return m;
	}

};//Colour_16


struct Colour_24
{
	unsigned char b,g,r,a;   //Can be 0 - 255

	int max_channel_value()
	{
		int m = 0;
		if(r > m){m = r;}
		if(g > m){m = g;}
		if(b > m){m = b;}
		return m;
	}

	int min_channel_value()
	{
		int m = 255;
		if(r < m){m = r;}
		if(g < m){m = g;}
		if(b < m){m = b;}
		return m;
	}

};//Colour_24

struct fColour_24
{
	float r,g,b,a;   //Can be 0 - 1
};//fColour_24

struct fColour_16
{
	float r,g,b,a;   //Can be 0 - 1
};//fColour_16

fColour_24 Colour256_to_fColour(Colour_24 c256)
{
	fColour_24 fc;
	fc.r = ((float)c256.r)/255.0f;
	fc.g = ((float)c256.g)/255.0f;
	fc.b = ((float)c256.b)/255.0f;
	fc.a = ((float)c256.a)/255.0f;

	return fc;
}// Colour256_to_fColour

Colour_24 fColour_to_Colour256(fColour_24 fc)
{
	Colour_24 c256;
	c256.r = floor(fc.r * 255.0f + 0.5);
	c256.g = floor(fc.g * 255.0f + 0.5);
	c256.b = floor(fc.b * 255.0f + 0.5);
	c256.a = floor(fc.a * 255.0f + 0.5);

	return c256;
}// fColour_to_Colour256

