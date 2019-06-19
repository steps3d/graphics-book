#include	<stdio.h>
#include	"TgaImage.h"

int main ()
{
	TgaImage image ( 100,100 );

	uint32_t	c1 = image.rgbToInt ( 0xFF, 0, 0 );
	uint32_t	c2 = image.rgbToInt ( 0, 0xFF, 0 );
	uint32_t	c3 = image.rgbToInt ( 0, 0, 0xFF );

	image.putPixel ( 10, 10, c1 );
	image.putPixel ( 11, 10, c1 );
	image.putPixel ( 12, 10, c1 );
	image.putPixel ( 13, 10, c1 );
	image.putPixel ( 20, 10, c3 );

	for ( int i = 15; i < 50; i++ )
		image.putPixel ( i, i, c2 );

	printf ( "put: %06x get: %06x\n", c1, image.getPixel ( 10, 10 ) );
	printf ( "put: %06x get: %06x\n", c2, image.getPixel ( 15, 15 ) );
	printf ( "put: %06x get: %06x\n", c3, image.getPixel ( 20, 10 ) );

	image.writeToFile ( "test.tga" );

	return 0;
}
