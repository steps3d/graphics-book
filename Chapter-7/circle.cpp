#include <stdio.h>
#include "TgaImage.h"
#include "Timer.h"

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
	Timer	t;

	TgaImage	image ( 500, 500 );

	drawCircle2 ( image, 250, 250, 100, image.rgbToInt ( 0xFF, 0, 0xFF ) );

	image.writeToFile ( "circle.tga" );

	printf ( "%lf\n", t.time ());

	return 0;
}
