#include	<math.h>
#include	"TgaImage.h"

template <typename T>
T abs ( T x )
{
	return x > 0 ? x : -x;
}

void drawLine1 ( TgaImage& image, int xa, int ya, int xb, int yb, 
                 uint32_t color )
{
	float k = (float)(yb-ya)/(float)(xb - xa);
	float b = ya - k * xa;

	for ( int x = xa; x <= xb; x++ )
		image.putPixel ( x, round ( k*x + b ), color );
}

void drawLine2 ( TgaImage& image, int xa, int ya, int xb, int yb, 
                 uint32_t color )
{
	float k = (float)(yb-ya)/(float)(xb - xa);
	float b = ya - k*xa;
	float y = ya;

	for ( int x = xa; x <= xb; x++, y += k )
		image.putPixel ( x, round ( y ), color );
}

void drawLine3 ( TgaImage& image, int xa, int ya, int xb, int yb, 
                 uint32_t color )
{
	float k = (float)(yb-ya)/(float)(xb - xa);
	float c = 0;
	int   y = (int)ya;

	image.putPixel ( xa, ya, color );
	
	for ( int x = xa+1; x <= xb; x++ )
	{
		if ( (c += k) > 0.5f )
		{
			c--;
			y++;
		}

		image.putPixel ( x, y, color );
	}
}

void drawLine4 ( TgaImage& image, int xa, int ya, int xb, int yb, 
                 uint32_t color )
{
	float k = (float)(yb-ya)/(float)(xb - xa);
	float d = 2*k - 1;
	int   y = (int)ya;

	image.putPixel ( xa, ya, color );
	
	for ( int x = xa+1; x <= xb; x++ )
	{
		if ( d > 0 )
		{
			d += 2*k + 2;
			y++;
		}
		else
			d += 2*k;

		image.putPixel ( x, y, color );
	}
}

void drawLine5 ( TgaImage& image, int xa, int ya, int xb, int yb, 
                 uint32_t color )
{
	int dx = xb - xa;
	int dy = yb - ya;
	int d  = (dy<<1) - dx;
	int d1 = dy << 1;
	int d2 = (dy - dx) << 1;
	int y  = ya;

	image.putPixel ( xa, ya, color );
	
	for ( int x = xa+1; x <= xb; x++ )
	{
		if ( d > 0 )
		{
			d += d2;
			y++;
		}
		else
			d += d1;

		image.putPixel ( x, y, color );
	}
}

void drawLine4Conn ( TgaImage& image, int xa, int ya, int xb, int yb, uint32_t color )
{
	int dx = xb - xa;
	int dy = yb - ya;
	int d  = (dy<<1) - dx;
	int d1 = dy << 1;
	int d2 = (dy - dx) << 1;
	int x  = xa;
	int y  = ya;

	image.putPixel ( xa, ya, color );
	for ( int i = 1; i <= dy + dx; i++ )
	{
		if ( d > 0 )
		{
			d += d2;
			y++;
		}
		else
		{
			d += d1;
			x++;
		}

		image.putPixel ( x, y, color );
	}
}

void drawLine ( TgaImage& image, int xa, int ya, int xb, int yb, 
                uint32_t color )
{
	int dx = abs(xb - xa);
	int dy = abs(yb - ya);
	int sx = xb >= xa ? 1 : -1;    // sign of xb-xa
	int sy = yb >= ya ? 1 : -1;    // sign of yb-ya

	if ( dy <= dx )
	{
		int d  = (dy<<1) - dx;
		int d1 = dy << 1;
		int d2 = (dy - dx) << 1;
		int x  = xa + sx;
		int y  = ya;

		image.putPixel ( xa, ya, color );
		for ( int i = 1; i <= dx; i++, x += sx )
		{
			if ( d > 0 )
			{
				d += d2;
				y += sy;
			}
			else
				d += d1;

			image.putPixel ( x, y, color );
		}
	}
	else
	{
		int d  = (dx<<1) - dy;
		int d1 = dx << 1;
		int d2 = (dx - dy) << 1;
		int x  = xa;
		int y  = ya + sy;

		image.putPixel ( xa, ya, color );
		for ( int i = 1; i <= dy; i++, y += sy )
		{
			if ( d > 0 )
			{
				d += d2;
				x += sx;
			}
			else
				d += d1;

			image.putPixel ( x, y, color );
		}
	}
}

int main ()
{
	TgaImage image ( 512, 512 );

	drawLine ( image, 0, 0, 300, 100, image.rgbToInt ( 0xFF, 0xFF, 0 ) );

	image.writeToFile ( "line1.tga" );
	image.clear       ();

	drawLine2 ( image, 0, 0, 300, 100, image.rgbToInt ( 0xFF, 0xFF, 0 ) );

	image.writeToFile ( "line2.tga" );
	image.clear       ();

	drawLine3 ( image, 0, 0, 300, 100, image.rgbToInt ( 0xFF, 0xFF, 0 ) );

	image.writeToFile ( "line3.tga" );
	image.clear       ();

	drawLine4 ( image, 0, 0, 300, 100, image.rgbToInt ( 0xFF, 0xFF, 0 ) );

	image.writeToFile ( "line4.tga" );
	image.clear       ();

	drawLine5 ( image, 0, 0, 300, 100, image.rgbToInt ( 0xFF, 0xFF, 0 ) );

	image.writeToFile ( "line5.tga" );
	image.clear       ();

	drawLine4Conn ( image, 0, 0, 300, 100, image.rgbToInt ( 0xFF, 0xFF, 0 ) );

	image.writeToFile ( "line4conn.tga" );
	image.clear       ();

	drawLine ( image, 100, 300, 0, 0, image.rgbToInt ( 0xFF, 0xFF, 0 ) );

	image.writeToFile ( "line-any.tga" );
	image.clear       ();

	return 0;
}
