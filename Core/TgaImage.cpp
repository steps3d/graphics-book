#include    <memory.h>
#ifdef	_WIN32
    #include	<fcntl.h>
    #include	<io.h>
    #include	<sys/stat.h>
#else
    #include	<unistd.h>
    #include	<sys/types.h>
    #include	<sys/stat.h>
    #include	<fcntl.h>
    #define	O_BINARY	0
#endif

#include "TgaImage.h"

TgaImage::TgaImage ( int w, int h ) : data(nullptr), width(w), height(h)
{
	int	numBytes = width*height*3;

	data = new uint8_t [numBytes];

	memset ( data, 0, numBytes);
}

TgaImage::~TgaImage () 
{
	delete [] data;
}

bool TgaImage::writeToFile ( const char * fileName )
{
	int	file = open ( fileName, O_WRONLY | O_BINARY | O_CREAT, S_IWRITE );
	
	if ( file == -1 )
		return false;
	
	TgaHeader	header;
	int			numBytes = width * height * 3;
	
	memset ( &header, '\0', sizeof ( header ) );
	
	header.imageType = 2;              // unencoded image
	header.width     = width;
	header.height    = height;
	header.pixelSize = 24;
	
	write ( file, &header, sizeof ( header ) );
	write ( file, data, numBytes );
	close ( file );
	
	return true;
}

void     TgaImage::clear ()
{
	memset ( data, '\0', width * height * 3 );
}

void 	TgaImage :: setRgbData ( const uint8_t * ptr )
{
	int	numPixels = width * height;
	int	offs      = 0;
	
	for ( int i = 0; i < numPixels; i++, offs += 3 )
	{
		data [offs]   = ptr [offs+2];
		data [offs+1] = ptr [offs+1];
		data [offs+2] = ptr [offs];
	}
}
