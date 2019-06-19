#include "TgaImage.h"

/*
void boundaryFill ( TgaImage& image, int x, int y, uint32_t borderColor, 
                    uint32_t fillColor )
{
  uint32_t c = image.getPixel ( x, y );
  
  if ( (c != borderColor) && (c != fillColor) )
  {
    image.putPixel     ( x, y, fillColor );
	
    boundaryFill ( image, x - 1, y, borderColor, fillColor );
    boundaryFill ( image, x + 1, y, borderColor, fillColor );
    boundaryFill ( image, x, y - 1, borderColor, fillColor );
    boundaryFill ( image, x, y + 1, borderColor, fillColor );
  }
}
*/

int lineFill ( TgaImage& image, 
    int xx, int y, int dir, int prevXl, int prevXr, 
    uint32_t borderColor, uint32_t fillColor )
{
	int 		xl = xx;
	int 		xr = xx;
	uint32_t  c;

	do
		c = image.getPixel ( --xl, y );
	while ( (c != borderColor) && (c != fillColor) );

	do
		c = image.getPixel ( ++xr, y );
	while ( (c != borderColor) && (c != fillColor) );

	xl++;
	xr--;

					// fill the span
	for ( int x = xl; x <= xr; x++ )
		image.putPixel ( x, y, fillColor );

	if ( y + dir >= 0 && y + dir < image.getHeight () )
		for ( int x = xl; x <= xr; x++ )
		{
			c = image.getPixel ( x, y + dir );
			if ( (c != borderColor) && (c != fillColor) )
				x = lineFill ( image, x, y + dir, dir, xl, xr, 
						   borderColor, fillColor );
		}

	if ( y - dir >= 0 && y - dir <= image.getHeight () )
		for ( int x = xl; x < prevXl; x++ )
		{
			c = image.getPixel ( x, y - dir );
			if ( (c != borderColor) && (c != fillColor) )
				x = lineFill ( image, x, y - dir, -dir, xl, xr, 
						   borderColor, fillColor );
		}

	for ( int x = prevXr; x < xr; x++ )
	{
		c = image.getPixel ( x, y - dir );
		if ( (c != borderColor) && (c != fillColor) )
			x = lineFill ( image, x, y - dir, -dir, xl, xr, 
						   borderColor, fillColor );
	}

	return xr;
}

void boundaryFill ( TgaImage& image, int x, int y, uint32_t borderColor, 
                    uint32_t fillColor )
{
	lineFill ( image, x, y, 1, x, x, borderColor, fillColor );
}

void drawCirclePoints ( TgaImage& image, int xc, int yc, int x, int y, 
                        uint32_t color )
{
  image.putPixel ( xc + x, yc + y, color );
  image.putPixel ( xc + y, yc + x, color );
  image.putPixel ( xc + y, yc - x, color );
  image.putPixel ( xc + x, yc - y, color );
  image.putPixel ( xc - x, yc - y, color );
  image.putPixel ( xc - y, yc - x, color );
  image.putPixel ( xc - y, yc + x, color );
  image.putPixel ( xc - x, yc + y, color );
}

void drawCircle1 ( TgaImage& image, int xc, int yc, int r, uint32_t color )
{
  int   x = 0;
  int   y = r;
  float d = 1.25f - r;
  
  drawCirclePoints ( image, xc, yc, x, y, color );
  
  while ( y > x )
  {
    if ( d < 0 )
    {
      d += 2*x + 3;
      x++;
    }
    else
    {
      d += 2*(x-y)+5;
      x++;
      y--;
    }
    drawCirclePoints ( image, xc, yc, x, y, color );
  }
}

void drawCircle2 ( TgaImage& image, int xc, int yc, int r, 
                  uint32_t color )
{
  int x      = 0;
  int y      = r;
  int d      = 1 - r;
  int delta1 = 3;
  int delta2 = -2*r+5;
  
  drawCirclePoints ( image, xc, yc, x, y, color );
  
  while ( y > x )
  {
    if ( d < 0 )
    {
      d      += delta1;
      delta1 += 2;
      delta2 += 2;
      x++;
    }
    else
    {
      d      += delta2;
      delta1 += 2;
      delta2 += 4;
      x++;
      y--;
    }
    
    drawCirclePoints ( image, xc, yc, x, y, color );
  }
}

int main ()
{
	TgaImage	image ( 500, 500 );

	drawCircle2  ( image, 250, 250, 120, image.rgbToInt ( 0xFF, 0, 0xFF ) );
	drawCircle2  ( image, 320, 250, 40,  image.rgbToInt ( 0xFF, 0, 0xFF ) );

	image.writeToFile ( "fill-in.tga" );

	boundaryFill ( image, 250, 250, image.rgbToInt ( 0xFF, 0, 0xFF ), image.rgbToInt ( 0, 0, 0xFF ) );
	
	image.writeToFile ( "fill.tga" );

	return 0;
}
