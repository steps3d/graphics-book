//
// Texture wrapper class for OpenGL 3.3 and higher
// Requires DevIL
//
// Author: Alexey V. Boreskov
//

#include	<stdlib.h>
#include	<string>
#include	"Texture.h"
#include	"stringUtils.h"
#include	"SOIL.h"
#include	"stb_image_aug.h"
#include	"TgaImage.h"
#include	"Data.h"

///	A bunch of DirectDraw Surface structures and flags
typedef struct {
    unsigned int    dwMagic;
    unsigned int    dwSize;
    unsigned int    dwFlags;
    unsigned int    dwHeight;
    unsigned int    dwWidth;
    unsigned int    dwPitchOrLinearSize;
    unsigned int    dwDepth;
    unsigned int    dwMipMapCount;
    unsigned int    dwReserved1[ 11 ];

    //  DDPIXELFORMAT
    struct {
      unsigned int    dwSize;
      unsigned int    dwFlags;
      unsigned int    dwFourCC;
      unsigned int    dwRGBBitCount;
      unsigned int    dwRBitMask;
      unsigned int    dwGBitMask;
      unsigned int    dwBBitMask;
      unsigned int    dwAlphaBitMask;
    }               sPixelFormat;

    //  DDCAPS2
    struct {
      unsigned int    dwCaps1;
      unsigned int    dwCaps2;
      unsigned int    dwDDSX;
      unsigned int    dwReserved;
    }               sCaps;
    unsigned int    dwReserved2;
} DDS_header ;

//	the following constants were copied directly off the MSDN website

//	The dwFlags member of the original DDSURFACEDESC2 structure
//	can be set to one or more of the following values.
#define DDSD_CAPS	0x00000001
#define DDSD_HEIGHT	0x00000002
#define DDSD_WIDTH	0x00000004
#define DDSD_PITCH	0x00000008
#define DDSD_PIXELFORMAT	0x00001000
#define DDSD_MIPMAPCOUNT	0x00020000
#define DDSD_LINEARSIZE	0x00080000
#define DDSD_DEPTH	0x00800000

//	DirectDraw Pixel Format
#define DDPF_ALPHAPIXELS	0x00000001
#define DDPF_FOURCC	0x00000004
#define DDPF_RGB	0x00000040

//	The dwCaps1 member of the DDSCAPS2 structure can be
//	set to one or more of the following values.
#define DDSCAPS_COMPLEX	0x00000008
#define DDSCAPS_TEXTURE	0x00001000
#define DDSCAPS_MIPMAP	0x00400000

//	The dwCaps2 member of the DDSCAPS2 structure can be
//	set to one or more of the following values.
#define DDSCAPS2_CUBEMAP	0x00000200
#define DDSCAPS2_CUBEMAP_POSITIVEX	0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX	0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY	0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY	0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ	0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ	0x00008000
#define DDSCAPS2_VOLUME	0x00200000


Texture :: Texture ( bool theAutoMipmaps )
{
	id          = 0;
	target      = 0;
	width       = 0;
	height      = 0;
	depth       = 0;
	autoMipmaps = theAutoMipmaps;
	unit        = -1;
}

Texture :: ~Texture ()
{
	destroy ();
}

void	Texture :: destroy ()
{
	if ( id == 0 )
		return;

	glDeleteTextures ( 1, &id );

	id          = 0;
	target      = 0;
	unit        = -1;
}

void	Texture :: initMips () 
{
	if ( autoMipmaps )
		glGenerateMipmap ( target );			

	if ( autoMipmaps )
	{
		glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else
	{
		glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
}

bool	Texture :: create1D ( int theWidth, GLenum theFormat, GLenum theIntFmt )		// use TexFormat as input
{
	target = GL_TEXTURE_1D;
	width  = theWidth;
	height = 1;
	depth  = 1;
	
	glGenTextures   ( 1, &id );
	glBindTexture   ( target, id );
	glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                  // set 1-byte alignment
	glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set default params for texture
	glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage1D    ( target, 0, theIntFmt, width, 0, theFormat, GL_UNSIGNED_BYTE, nullptr );
	glBindTexture   ( target, 0 );

	return true;
}

bool	Texture :: create2D ( int theWidth, int theHeight, GLenum theFormat, GLenum theIntFmt )
{
	target = GL_TEXTURE_2D;
	width  = theWidth;
	height = theHeight;
	depth  = 1;
	
	glGenTextures   ( 1, &id );
	glBindTexture   ( target, id );
	glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                  // set 1-byte alignment
	glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set default params for texture
	glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D    ( target, 0, theIntFmt, width, height, 0, theFormat, GL_UNSIGNED_BYTE, nullptr );
	glBindTexture   ( target, 0 );

	return true;
}
/*
bool	Texture :: create2DArray ( int theWidth, int theHeight, int numLayers, GLenum theFormat, GLenum theIntFmt )
{
	target = GL_TEXTURE_2D_ARRAY;
	width  = theWidth;
	height = theHeight;
	depth  = numLayers;
	
	glGenTextures   ( 1, &id );
	glBindTexture   ( target, id );
	glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                  // set 1-byte alignment
	glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set default params for texture
	glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage3D    ( target, 0, theIntFmt, width, height, depth, 0, theFormat, GL_UNSIGNED_BYTE, nullptr );
	glBindTexture   ( target, 0 );

	return true;
}
*/
bool	Texture :: createRectangle ( int theWidth, int theHeight, GLenum theFormat, GLenum theIntFmt )
{
	target      = GL_TEXTURE_RECTANGLE;
	autoMipmaps = false;
	width       = theWidth;
	height      = theWidth;
	depth       = 1;
	
	glGenTextures   ( 1, &id );
	glBindTexture   ( target, id );
	glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );     // set default params for texture
	glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                  // set 1-byte alignment
	glTexImage2D    ( target, 0, theIntFmt, width, height, 0, theFormat, GL_UNSIGNED_BYTE, nullptr );
	glBindTexture   ( target, 0 );

	return true;
}

bool	Texture :: createDepthRect ( int theWidth, int theHeight, GLenum theFormat, GLenum theIntFmt )
{
	target      = GL_TEXTURE_RECTANGLE;
	autoMipmaps = false;
	width       = theWidth;
	height      = theWidth;
	depth       = 1;
	
	glGenTextures   ( 1, &id );
	glBindTexture   ( target, id );
	glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );     // set default params for texture
	glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexImage2D    ( target, 0, theIntFmt, width, height, 0, theFormat, GL_UNSIGNED_BYTE, nullptr );
	glBindTexture   ( target, 0 );

	return true;
}

bool	Texture :: create3D ( int theWidth, int theHeight, int theDepth, GLenum theFormat, GLenum theIntFmt )
{
	target = GL_TEXTURE_3D;
	width  = theWidth;
	height = theHeight;
	depth  = theDepth;
	
	glGenTextures   ( 1, &id );
	glBindTexture   ( target, id );
	glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                         // set 1-byte alignment
	glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set default params for texture
	glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage3D    ( target, 0, theIntFmt, width, height, depth, 0, theFormat, GL_UNSIGNED_BYTE, nullptr );
	glBindTexture   ( target, 0 );

	return true;
}

bool	Texture :: createCubemap ( int theWidth, GLenum theFormat, GLenum theIntFmt )
{
	target = GL_TEXTURE_CUBE_MAP;				// GL_TEXTURE_2D;
	width  = theWidth;
	height = theWidth;
	depth  = 1;
	
	glGenTextures   ( 1, &id );
	glBindTexture   ( target, id );
	glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                  // set 1-byte alignment
	glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set default params for texture
	glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, theIntFmt, width, width, 0, theFormat, GL_UNSIGNED_BYTE, nullptr );

	glBindTexture   ( target, 0 );

	return true;
}

									// create empty texture arrays
bool	Texture :: createArray1D ( int theWidth, int numSlices, GLenum theFormat, GLenum theIntFmt )
{
	target = GL_TEXTURE_1D_ARRAY;
	width  = theWidth;
	height = numSlices;
	depth  = 1;
	
	glGenTextures   ( 1, &id );
	glBindTexture   ( target, id );
	glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                         // set 1-byte alignment
	glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set default params for texture
	glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D    ( target, 0, theIntFmt, width, numSlices, 0, theFormat, GL_UNSIGNED_BYTE, nullptr );
	glBindTexture   ( target, 0 );

	return true;
}

bool	Texture :: createArray2D ( int theWidth, int theHeight, int numSlices, GLenum theFormat, GLenum theIntFmt )
{
	target = GL_TEXTURE_2D_ARRAY;
	width  = theWidth;
	height = theHeight;
	depth  = numSlices;
	
	glGenTextures   ( 1, &id );
	glBindTexture   ( target, id );
	glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                         // set 1-byte alignment
	glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set default params for texture
	glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage3D    ( target, 0, theIntFmt, width, height, numSlices, 0, theFormat, GL_UNSIGNED_BYTE, nullptr );
	glBindTexture   ( target, 0 );

	return true;
}

bool	Texture :: loadRect ( const std::string& fileName )
{
	target = GL_TEXTURE_RECTANGLE;
	depth  = 1;
	id     = SOIL_load_OGL_texture ( fileName.c_str (), 
					SOIL_LOAD_AUTO,
					SOIL_CREATE_NEW_ID,
					SOIL_FLAG_TEXTURE_RECTANGLE | SOIL_FLAG_DDS_LOAD_DIRECT );
	
	if ( id <= 0 )
		return false;

	glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set default params for texture
	glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_REPEAT );
	
	glGetTexLevelParameteriv ( target, 0, GL_TEXTURE_WIDTH,  &width );
	glGetTexLevelParameteriv ( target, 0, GL_TEXTURE_HEIGHT, &height );
	
	glBindTexture  ( target, 0 );

	return true;
}


bool	Texture :: load2D ( const std::string& fileName )
{
	target = GL_TEXTURE_2D;
	depth  = 1;
	id     = SOIL_load_OGL_texture ( fileName.c_str (), 
					SOIL_LOAD_AUTO,
					SOIL_CREATE_NEW_ID,
					SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_DDS_LOAD_DIRECT );
	
	if ( id <= 0 )
		return false;

	glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set default params for texture
	glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_REPEAT );
	
	if ( autoMipmaps )
		glGenerateMipmap ( target );
	
	if ( autoMipmaps )
	{
		glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else
	{
		glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	
	glGetTexLevelParameteriv ( target, 0, GL_TEXTURE_WIDTH,  &width );
	glGetTexLevelParameteriv ( target, 0, GL_TEXTURE_HEIGHT, &height );
	
	glBindTexture  ( target, 0 );

	return true;
}

bool	Texture :: load2DHdr ( const std::string& fileName )
{
//    stbi_set_flip_vertically_on_load ( true );
	int 	numComponents;
	float * data = stbi_loadf( fileName.c_str (), &width, &height, &numComponents, 0 );
	
	if ( data == nullptr )
	{
			// check for floating-point DDS 
		Data	     data ( fileName );
		if ( (size_t)data.getLength () < sizeof ( DDS_header ) )
			return false;

		DDS_header * hdr = (DDS_header *) data.getPtr ();

		if ( hdr->dwMagic != (('D' << 0) | ('D' << 8) | ('S' << 16) | (' ' << 24)) ) 
			return false;
		
		if ( hdr->dwSize != 124 ) 
			return false;

		unsigned	flags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;

		if ( (hdr->dwFlags & flags) != flags ) 
			return false;

		if ( hdr->sPixelFormat.dwSize != 32 ) 
			return false;

		flags = DDPF_FOURCC | DDPF_RGB;

		if ( (hdr->sPixelFormat.dwFlags & flags) == 0 ) 
			return false;

		if ( (hdr->sCaps.dwCaps1 & DDSCAPS_TEXTURE) == 0 ) 
			return false;

		if ( (hdr->sCaps.dwCaps2 & DDSCAPS2_CUBEMAP) != 0 )
			return false;
		
		target = GL_TEXTURE_2D;
		width  = hdr->dwWidth;
		height = hdr->dwHeight;
		depth  = 1;
	
		if ( hdr->sPixelFormat.dwFourCC == 113 )	// GL_RGBA16F
		{
			glGenTextures ( 1, &id );
			glBindTexture ( target, id );
			glTexImage2D  ( GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_HALF_FLOAT, hdr+1 );
		
			goto ok;
		}
		else if ( hdr->sPixelFormat.dwFourCC == 111 )	// GL_R16F
		{
			glGenTextures ( 1, &id );
			glBindTexture ( target, id );
			glTexImage2D  ( GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_HALF_FLOAT, hdr+1 );
		
			goto ok;
		}
		else if ( hdr->sPixelFormat.dwFourCC == 112 )	// GL_RG16F
		{
			glGenTextures ( 1, &id );
			glBindTexture ( target, id );
			glTexImage2D  ( GL_TEXTURE_2D, 0, GL_RG16F, width, height, 0, GL_RG, GL_HALF_FLOAT, hdr+1 );
		
			goto ok;
		}
		else if ( hdr->sPixelFormat.dwFourCC == 114 )	// GL_R32F
		{
			glGenTextures ( 1, &id );
			glBindTexture ( target, id );
			glTexImage2D  ( GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, hdr+1 );
		
			goto ok;
		}
		else if ( hdr->sPixelFormat.dwFourCC == 115 )	// GL_RG32F
		{
			glGenTextures ( 1, &id );
			glBindTexture ( target, id );
			glTexImage2D  ( GL_TEXTURE_2D, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, hdr+1 );
		
			goto ok;
		}
		else if ( hdr->sPixelFormat.dwFourCC == 116 )	// GL_RGBA32F
		{
			glGenTextures ( 1, &id );
			glBindTexture ( target, id );
			glTexImage2D  ( GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, hdr+1 );
		
			goto ok;
		}

		return false;
	}
	
	target = GL_TEXTURE_2D;
	depth  = 1;
	
	glGenTextures ( 1, &id );
	glBindTexture ( target, id );
	glTexImage2D  ( GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data );

	stbi_image_free ( data );
ok:;

	glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set default params for texture
	glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_REPEAT );
	
	if ( autoMipmaps )
		glGenerateMipmap ( target );
	
	if ( autoMipmaps )
	{
		glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else
	{
		glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	
	glBindTexture  ( target, 0 );

	return true;
}

bool	Texture :: loadCubemapHdr ( const std::string& fileName )
{
			// check for floating-point DDS 
	Data	     data ( fileName );
	
	if ( (size_t)data.getLength () < sizeof ( DDS_header ) )
		return false;

	DDS_header * hdr = (DDS_header *) data.getPtr ();

	if ( hdr->dwMagic != (('D' << 0) | ('D' << 8) | ('S' << 16) | (' ' << 24)) ) 
		return false;
	
	if ( hdr->dwSize != 124 ) 
		return false;

	unsigned	flags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;

	if ( (hdr->dwFlags & flags) != flags ) 
		return false;

	if ( hdr->sPixelFormat.dwSize != 32 ) 
		return false;

	flags = DDPF_FOURCC | DDPF_RGB;

	if ( (hdr->sPixelFormat.dwFlags & flags) == 0 ) 
		return false;

	if ( (hdr->sCaps.dwCaps1 & DDSCAPS_TEXTURE) == 0 ) 
		return false;

	if ( (hdr->sCaps.dwCaps2 & DDSCAPS2_CUBEMAP) == 0 )
		return false;
	
	uint8_t * ptr = (uint8_t *)(hdr + 1);		// pointer to raw image data
	
	if ( hdr->sPixelFormat.dwFourCC == 113 )	// GL_RGBA16F
	{
		target = GL_TEXTURE_CUBE_MAP;
		width  = hdr->dwWidth;
		height = hdr->dwHeight;
		depth  = 1;

		glGenTextures ( 1, &id );
		glBindTexture ( target, id );
		
		for ( int i = 0; i < 6; i++ )
			glTexImage2D  ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_HALF_FLOAT, ptr + i * 2 * width * height );
	}

	glTexParameterf ( target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf ( target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri ( target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	
	if ( autoMipmaps )
		glGenerateMipmap ( target );
	
	if ( autoMipmaps )
	{
		glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else
	{
		glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	
	glBindTexture  ( target, 0 );

	return true;
}

bool	Texture :: load3D ( const std::string& fileName )
{
	target = GL_TEXTURE_3D;
	id     = SOIL_load_OGL_texture ( fileName.c_str (), 
					SOIL_LOAD_AUTO,
					SOIL_CREATE_NEW_ID,
					SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_DDS_LOAD_DIRECT );
	
	if ( id <= 0 )
		return false;

	glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set default params for texture
	glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri ( target, GL_TEXTURE_WRAP_R, GL_REPEAT );
	
	if ( autoMipmaps )
		glGenerateMipmap ( target );
	
	if ( autoMipmaps )
	{
		glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else
	{
		glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	
	glGetTexLevelParameteriv ( target, 0, GL_TEXTURE_WIDTH,  &width  );
	glGetTexLevelParameteriv ( target, 0, GL_TEXTURE_HEIGHT, &height );
	glGetTexLevelParameteriv ( target, 0, GL_TEXTURE_DEPTH,  &depth  );
	
	glBindTexture  ( target, 0 );

	return true;
}

bool	Texture :: loadCubemap ( const std::string& fileName )
{
	target = GL_TEXTURE_CUBE_MAP;
	id     = SOIL_load_OGL_single_cubemap(
			fileName.c_str (),
			SOIL_DDS_CUBEMAP_FACE_ORDER,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_POWER_OF_TWO
			| SOIL_FLAG_MIPMAPS
			//| SOIL_FLAG_COMPRESS_TO_DXT
			//| SOIL_FLAG_TEXTURE_REPEATS
			//| SOIL_FLAG_INVERT_Y
			| SOIL_FLAG_DDS_LOAD_DIRECT
			);

	if ( id <= 0 )
		return false;

/*	id     = SOIL_load_OGL_texture ( fileName, 
					SOIL_LOAD_AUTO,
					SOIL_CREATE_NEW_ID,
					SOIL_TEXTURE_CUBE_MAP | SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_DDS_LOAD_DIRECT );
*/	
	glBindTexture   ( target, id );
	glTexParameterf ( target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf ( target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri ( target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

	if ( autoMipmaps )
		glGenerateMipmap ( target );
	
	setDefaultFilter ();
	
	glGetTexLevelParameteriv ( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_TEXTURE_WIDTH, &width);
	glBindTexture            ( target, 0 );

	height = width;
	depth  = 1;
	
	return true;
}

bool	Texture :: loadCubemapFromNames ( const char * fileNames )
{
	const char * seps = ",;";
	Tokenizer	 tok ( fileNames, seps );
	std::string	 names [6];
	
	for ( int i = 0; i < 6; i++ )
	{
		names [i] = tok.next ();
		
		if ( names [i].empty () )
			return false;
	}
	
	return loadCubemap ( names [0].c_str (), names [1].c_str (), names [2].c_str (), names [3].c_str (), 
	                     names [4].c_str (), names [5].c_str () );
}

bool	Texture :: loadCubemap ( const char * f1, const char * f2, const char * f3, const char * f4, const char * f5, const char * f6 )
{
	target = GL_TEXTURE_CUBE_MAP;
	id     = SOIL_load_OGL_cubemap ( f1, f2, f3, f4, f5, f6, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS );

	if ( id <= 0 )
		return false;

	initMips ();
	
	glGetTexLevelParameteriv ( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_TEXTURE_WIDTH, &height );
	
	glBindTexture  ( target, 0 );

	height = width;
	depth  = 1;
	
	return true;
}

bool	Texture :: load1DRaw ( int theWidth, GLenum theFormat, GLenum theIntFmt, GLenum dataType, const void * theData )
{
	target = GL_TEXTURE_1D;
	width  = theWidth;
	height = 1;
	depth  = 1;
	
	glGenTextures   ( 1, &id );
	glBindTexture   ( target, id );
	glGenTextures   ( 1, &id );
	glBindTexture   ( target, id );
	glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                  // set 1-byte alignment
	glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set default params for texture
	glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage1D    ( target, 0, theIntFmt, width, 0, theFormat, dataType, theData );
	glBindTexture   ( target, 0 );

	return true;
}

bool	Texture :: load2DRaw ( int theWidth, int theHeight, GLenum theFormat, GLenum theIntFmt, GLenum dataType, const void * theData )
{
	target = GL_TEXTURE_2D;
	width  = theWidth;
	height = theHeight;
	depth  = 1;
	
	glGenTextures   ( 1, &id );
	glBindTexture   ( target, id );
	glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                  // set 1-byte alignment
	glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set default params for texture
	glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D    ( target, 0, theIntFmt, width, height, 0, theFormat, dataType, theData );
	glBindTexture   ( target, 0 );
	
	return true;
}

bool	Texture :: loadRectRaw ( int theWidth, int theHeight, GLenum theFormat, GLenum theIntFmt, GLenum dataType, const void * theData )
{
	target      = GL_TEXTURE_RECTANGLE;
	autoMipmaps = false;
	width       = theWidth;
	height      = theHeight;
	depth       = 1;
	
	glGenTextures   ( 1, &id );
	glBindTexture   ( target, id );
	glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                  // set 1-byte alignment
	glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );    // set default params for texture
	glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D    ( target, 0, theIntFmt, width, height, 0, theFormat, dataType, theData );
	glBindTexture   ( target, 0 );
	
	return true;
}

bool	Texture :: load3DRaw ( int theWidth, int theHeight, int theDepth, GLenum theFormat, GLenum theIntFmt, GLenum dataType, const void * theData )
{
	target = GL_TEXTURE_3D;
	width  = theWidth;
	height = theHeight;
	depth  = theDepth;
	
	glGenTextures   ( 1, &id );
	glBindTexture   ( target, id );
	glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                  // set 1-byte alignment
	glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set default params for texture
	glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage3D    ( target, 0, theIntFmt, width, height, depth, 0, theFormat, dataType, theData );
	glBindTexture   ( target, 0 );

	return true;
}

bool	Texture :: load2DArray ( int theWidth, int theHeight, int numSlices, GLenum theFormat, GLenum theIntFmt, GLenum dataType, const void * theData )
{
	target = GL_TEXTURE_2D_ARRAY;
	width  = theWidth;
	height = theHeight;
	depth  = numSlices;
	
	glGenTextures   ( 1, &id );
	glBindTexture   ( target, id );
	glPixelStorei   ( GL_UNPACK_ALIGNMENT, 1 );                  // set 1-byte alignment
	glTexParameteri ( target, GL_TEXTURE_WRAP_S, GL_REPEAT );    // set default params for texture
	glTexParameteri ( target, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage3D    ( target, 0, theIntFmt, width, height, numSlices, 0, theFormat, dataType, theData );
	glBindTexture   ( target, 0 );

	return true;
}

void	Texture :: getImage ( int level, GLenum format, GLenum type, void * pixels )
{
	glGetTexImage ( target, level, format, type, pixels );
}

void	Texture :: bind ( int texUnit )
{
	unit = texUnit;				// store the unit

	glActiveTexture ( GL_TEXTURE0 + texUnit );
	glBindTexture   ( target, id );
}

void	Texture :: unbind ()
{
	if ( unit != -1 )
	{
		glActiveTexture ( GL_TEXTURE0 + unit );
		glBindTexture ( target, 0 );

		unit = -1;			// not bound
	}
}

void	Texture :: buildMipmaps ()
{
	glTexParameteri  ( target, GL_TEXTURE_BASE_LEVEL, 0 );
	glGenerateMipmap ( target );						// should be bound
}

bool	Texture :: saveToFile ( const std::string& fileName, GLenum format ) const
{
	if ( (target != GL_TEXTURE_1D) && (target != GL_TEXTURE_2D) && (target != GL_TEXTURE_RECTANGLE) )
		return false;

if ( format == GL_DEPTH_COMPONENT )
{

	float   * buf = (float   *) malloc ( width * height * sizeof ( float ) );
	uint8_t * ptr = (uint8_t *) malloc ( width * height * 3 );

	glGetTexImage ( target, 0, GL_DEPTH_COMPONENT, GL_FLOAT, buf );

	for ( int i = 0, k = 0; i < height; i++ )
		for ( int j = 0; j < width; j++)
		{
			uint8_t	value = (uint8_t)(255.0f * buf [k]);

			ptr[3*k]   = value;
			ptr[3*k+1] = value;
			ptr[3*k+2] = value;
			k++;
		}

	TgaImage	image ( width, height );
	
	image.setRgbData  ( ptr );				// it should do RGB->BGR mapping
	image.writeToFile ( fileName.c_str () );

	free ( buf );
	free ( ptr );

	return true;
}

	uint8_t * ptr = (uint8_t *) malloc ( width * height * 3 );
	
	glGetTexImage ( target, 0, GL_RGB, GL_UNSIGNED_BYTE, ptr );
	
	TgaImage	image ( width, height );
	
	image.setRgbData  ( ptr );				// it should do RGB->BGR mapping
	image.writeToFile ( fileName.c_str () );
	
	free ( ptr );

	return true;
}

void	Texture :: setSwizzle ( GLenum _red, GLenum _green, GLenum _blue, GLenum _alpha )
{
	int	swizzle [4] = { (int)_red, (int)_green, (int)_blue, (int)_alpha };

	glTexParameteriv ( target, GL_TEXTURE_SWIZZLE_RGBA, swizzle );
}

int	Texture :: maxSize ()
{
	int	maxSize;
	
	glGetIntegerv ( GL_MAX_TEXTURE_SIZE, &maxSize );
	
	return maxSize;
}

void	Texture :: setBaseLevel ( int level )
{
	glTexParameteri ( target, GL_TEXTURE_BASE_LEVEL, level );
}

void	Texture :: setMaxLevel ( int level )
{
	glTexParameteri ( target, GL_TEXTURE_MAX_LEVEL, level );
}
