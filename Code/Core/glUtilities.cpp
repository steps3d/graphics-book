//
// Simple OpenGL utility functions
//

#include	<GL/glew.h>
#include	<stdio.h>
#include	<memory.h>
#include	<malloc.h>
#include	"glUtilities.h"
#include	"TypeDefs.h"
#include	"TgaEncoder.h"

const char * getGlErrorString ()
{
	int error;
	
	if ( (error = glGetError()) != GL_NO_ERROR)
	{
		switch ( error )
		{
		case GL_NO_ERROR:				// no error, return nullptr
			return nullptr;
			
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";
			
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";
		
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";
			
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
			
		case GL_OUT_OF_MEMORY:
			return  "GL_OUT_OF_MEMORY";
			
		default:
			return "UNKNOWN ERROR";
		}
	}
	
	return nullptr;
}

const char * checkFramebuffer ()
{
	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	
	if ( status == GL_FRAMEBUFFER_COMPLETE )		// ok
		return nullptr;
		
	switch ( status )
	{
		case GL_FRAMEBUFFER_UNDEFINED:
			return  "GL_FRAMEBUFFER_UNDEFINED";
			
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
			
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
			
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
			
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
			
		case GL_FRAMEBUFFER_UNSUPPORTED:
			return "GL_FRAMEBUFFER_UNSUPPORTED";
			
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
			
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
			
		default:
			return "UNKNOWN ERROR";
	}

	return nullptr;
}

void checkGlError ( const char * title )
{
	const char * str = getGlErrorString ();

	if ( str != nullptr )
		printf ( "%s - OpenGL error: %s\n", title, str );
}

bool	saveScreenAsTga ( const char * fileName, int width, int height )
{
    byte * ptr = (byte *) malloc ( 3 * width * height );

    if ( ptr == nullptr )
        return false;

                                            // get image from window
    glReadBuffer ( GL_FRONT );
    glReadPixels ( 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, ptr );

                                            // now save it as 24-bit uncompressed tga
	TgaEncoder	encoder;
	bool		result = encoder.encode ( ptr, width , height, 3, fileName );
                                            // cleanup
    free ( ptr );

    return result;
}
