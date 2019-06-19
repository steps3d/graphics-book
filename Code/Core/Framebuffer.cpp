//
// C++ wrapper for framebuffer object
//
// Author: Alex V. Boreskoff <alexboreskoff@mtu-net.ru>, <steps3d@narod.ru>
//

#include	<assert.h>
#include	<stdlib.h>
#include	"Framebuffer.h"
#include	"Texture.h"
#include	"glUtilities.h"

FrameBuffer :: FrameBuffer  ()
{
	width         = 0;
	height        = 0;
	flags         = 0;
	frameBuffer   = 0;
	depthBuffer   = 0;
	stencilBuffer = 0;
	
	for ( int i = 0; i < 8; i++ )
		colorBuffer [i] = nullptr;
}

FrameBuffer :: ~FrameBuffer ()
{
	destroy ();
}

void	FrameBuffer :: destroy ()
{
	if ( depthBuffer != 0 )
		glDeleteRenderbuffers ( 1, &depthBuffer );

	if ( stencilBuffer != 0 )
		glDeleteRenderbuffers ( 1, &stencilBuffer );

	if ( frameBuffer != 0 )
		glDeleteFramebuffers ( 1, &frameBuffer );

	depthBuffer   = 0;
	stencilBuffer = 0;
	frameBuffer   = 0;
}

bool	FrameBuffer :: create ( int theWidth, int theHeight, int theFlags )
{
	width         = theWidth;
	height        = theHeight;
	flags         = theFlags;
	
	if ( width <= 0 || height <= 0 )
		return false;

	glGenFramebuffers ( 1, &frameBuffer );
	glBindFramebuffer ( GL_FRAMEBUFFER, frameBuffer );

	int	depthFormat   = 0;
	int stencilFormat = 0;

	if ( flags & depth16 )
		depthFormat = GL_DEPTH_COMPONENT16;
	else
	if ( flags & depth24 )
		depthFormat = GL_DEPTH_COMPONENT24;
	else
	if ( flags & depth32 )
		depthFormat = GL_DEPTH_COMPONENT32;

	if ( flags & stencil1 )
		stencilFormat = GL_STENCIL_INDEX1;
	else
	if ( flags & stencil4 )
		stencilFormat = GL_STENCIL_INDEX4;
	else
	if ( flags & stencil8 )
		stencilFormat = GL_STENCIL_INDEX8;
	else
	if ( flags & stencil16 )
		stencilFormat = GL_STENCIL_INDEX16;

	if ( depthFormat != 0 )
	{
		glGenRenderbuffers        ( 1, &depthBuffer );
		glBindRenderbuffer        ( GL_RENDERBUFFER, depthBuffer );
		glRenderbufferStorage     ( GL_RENDERBUFFER, depthFormat, width, height );
		glFramebufferRenderbuffer ( GL_FRAMEBUFFER,  GL_DEPTH_ATTACHMENT,
		                            GL_RENDERBUFFER, depthBuffer );
	}

	if ( stencilFormat != 0 )
	{
		glGenRenderbuffers        ( 1, &stencilBuffer );
		glBindRenderbuffer        ( GL_RENDERBUFFER, stencilBuffer );
		glRenderbufferStorage     ( GL_RENDERBUFFER, stencilFormat, width, height );
		glFramebufferRenderbuffer ( GL_FRAMEBUFFER,  GL_STENCIL_ATTACHMENT,
		                            GL_RENDERBUFFER, stencilBuffer );
	}

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	
	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );

	return status == GL_FRAMEBUFFER_COMPLETE;
}

bool	FrameBuffer :: isOk () const
{
	GLuint	currentFb;

	glGetIntegerv ( GL_FRAMEBUFFER_BINDING, (GLint *)&currentFb );

	if ( currentFb != frameBuffer )
	{
		glBindFramebuffer ( GL_FRAMEBUFFER, frameBuffer );
		glReadBuffer         ( GL_COLOR_ATTACHMENT0 );
	}

	int	 status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

	if ( currentFb != frameBuffer )
		glBindFramebuffer ( GL_FRAMEBUFFER, currentFb );

	return 	status == GL_FRAMEBUFFER_COMPLETE;
}

bool	FrameBuffer :: bind ()
{
	if ( frameBuffer == 0 )
		return false;

	glFlush ();
								// save current viewport
	glGetIntegerv     ( GL_VIEWPORT, saveViewport );
	glBindFramebuffer ( GL_FRAMEBUFFER, frameBuffer );
	glReadBuffer      ( GL_COLOR_ATTACHMENT0 );
	glDrawBuffer      ( GL_COLOR_ATTACHMENT0 );
	glViewport        ( 0, 0, getWidth (), getHeight () );

	return true;
}

bool	FrameBuffer :: bindFace ( int face )
{
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, colorBuffer [0] -> getId (), 0 );

	return bind ();
}

bool	FrameBuffer :: unbind ( bool genMipmaps, GLenum target )
{
	if ( frameBuffer == 0 )
		return false;

	glFlush ();

	if ( genMipmaps )
		glGenerateMipmap ( target );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glViewport        ( saveViewport [0], saveViewport [1], saveViewport [2], saveViewport [3] );

	return true;
}

bool	FrameBuffer :: attachColorTexture ( Texture * tex, int no )
{
	if ( frameBuffer == 0 )
		return false;

	if ( tex -> getTarget () != GL_TEXTURE_2D && tex -> getTarget () != GL_TEXTURE_RECTANGLE )
		return false;

	colorBuffer [no] = tex;
	
	glBindTexture          ( tex -> getTarget (), tex -> getId () );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + no, tex -> getTarget (), tex -> getId (), 0 );

	return true;
}

bool	FrameBuffer :: attachCubeMapFace ( Texture * tex, int face, int no, int mipLevel )
{
	if ( frameBuffer == 0 )
		return false;

	if ( tex -> getTarget () != GL_TEXTURE_CUBE_MAP )
		return false;
		
	if ( face < 0 || face > 5 )
		return false;
		
	colorBuffer [no] = tex;
	
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + no, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, tex -> getId (), mipLevel );

	return true;
}

bool	FrameBuffer :: attach3DTextureSlice ( Texture * tex, int zOffs, int no )
{
	if ( frameBuffer == 0 )
		return false;

	if ( tex -> getTarget () != GL_TEXTURE_3D )
		return false;
		
	colorBuffer [no] = tex;
	
	glBindTexture          ( GL_TEXTURE_3D, tex -> getId () );
	glFramebufferTexture3D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + no, GL_TEXTURE_3D, tex -> getId (), 0, zOffs );

	return true;
}

bool	FrameBuffer :: attachDepthTexture ( unsigned texId )
{
	if ( frameBuffer == 0 )
		return false;

	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthBuffer = texId, 0 );

	return true;
}

bool	FrameBuffer::attachDepthTexture ( Texture * tex )
{
	if ( frameBuffer == 0 )
		return false;

	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthBuffer = tex->getId (), 0 );

	return true;
}

Texture * FrameBuffer :: createColorTexture ( GLenum format, GLenum internalFormat, GLenum clamp, int filter )
{
	Texture * tex = new Texture ( filter == filterMipmap );
	
	if ( !tex -> create2D ( width, height, format, internalFormat ) )
	{
		delete tex;
		
		return nullptr;
	}
		
	tex -> bind ();
	
	if ( filter == filterNearest )
	{
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	}
	else
	if ( filter == filterLinear )
	{
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	}
	else
	if ( filter == filterMipmap )
	{
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	}
	
	glTexParameteri ( tex -> getTarget (), GL_TEXTURE_WRAP_S, clamp );
	glTexParameteri ( tex -> getTarget (), GL_TEXTURE_WRAP_T, clamp );

	tex -> unbind ();
	
	return tex;
}

Texture * FrameBuffer :: createColorTextureArray ( int numLayers, GLenum format, GLenum internalFormat, GLenum clamp, int filter )
{
	Texture * tex = new Texture ( filter == filterMipmap );
	
	if ( !tex -> createArray2D ( width, height, numLayers, format, internalFormat ) )
	{
		delete tex;
		
		return nullptr;
	}
		
	tex -> bind ();
	
	if ( filter == filterNearest )
	{
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	}
	else
	if ( filter == filterLinear )
	{
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	}
	else
	if ( filter == filterMipmap )
	{
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	}
	
	glTexParameteri ( tex -> getTarget (), GL_TEXTURE_WRAP_S, clamp );
	glTexParameteri ( tex -> getTarget (), GL_TEXTURE_WRAP_T, clamp );

	tex -> unbind ();
	
	return tex;
}


Texture * FrameBuffer :: createCubemapTexture ( GLenum format, GLenum internalFormat, GLenum clamp, int filter )
{
	if ( getWidth () != getHeight () )
		return 0;
		
	Texture * tex = new Texture ( filter == filterMipmap );
	
	if ( !tex -> createCubemap ( width, format, internalFormat ) )
	{
		delete tex;
		
		return nullptr;
	}
	
	tex -> bind ();
	
	glTexParameteri ( tex -> getTarget (), GL_TEXTURE_WRAP_S, clamp );
	glTexParameteri ( tex -> getTarget (), GL_TEXTURE_WRAP_T, clamp );
	glTexParameteri ( tex -> getTarget (), GL_TEXTURE_WRAP_R, clamp );
	
	if ( filter == filterNearest )
	{
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	}
	else
	if ( filter == filterLinear )
	{
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	}
	else
	if ( filter == filterMipmap )
	{
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	}
	
	tex -> unbind ();
	
    return tex;
}

Texture * FrameBuffer :: create3DTexture ( int depth, GLenum format, GLenum internalFormat, GLenum clamp, int filter )
{
	Texture * tex = new Texture ( filter == filterMipmap );
	
	if ( !tex -> create3D ( width, height, depth, format, internalFormat ) ) 
	{
		delete tex;
		
		return nullptr;
	}

	tex -> bind ();
	
	glTexParameteri ( tex -> getTarget (), GL_TEXTURE_WRAP_S, clamp );
	glTexParameteri ( tex -> getTarget (), GL_TEXTURE_WRAP_T, clamp );
	glTexParameteri ( tex -> getTarget (), GL_TEXTURE_WRAP_R, clamp );
	
	if ( filter == filterNearest )
	{
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	}
	else
	if ( filter == filterLinear )
	{
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	}
	else
	if ( filter == filterMipmap )
	{
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri ( tex -> getTarget (), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	}
	
	tex -> unbind ();
	
	return tex;
}

Texture * FrameBuffer :: createColorRectTexture ( GLenum format, GLenum internalFormat )
{
	Texture * tex = new Texture ( false );
	
	if ( !tex -> createRectangle ( width, height, format, internalFormat ) ) 
	{
		delete tex;
		
		return nullptr;
	}

    return tex;
}

bool	FrameBuffer :: drawBuffers ( int no )
{
	assert ( no >= 0 && no < maxColorAttachments () );
	
	if ( no < 0 || no >= maxColorAttachments () )
		return false;
		
	GLenum	buffers [16];
	
	for ( int i = 0; i < no; i++ )
		buffers [i] = GL_COLOR_ATTACHMENT0 + i;
		
	glDrawBuffers ( no, buffers );
	
	return true;
}

void	FrameBuffer :: buildMipmaps ( int no ) const
{
									// be sure we're unbound
	Texture * tex = getColorBuffer ( no );
	
	tex -> bind ();
	tex -> buildMipmaps ();
	tex -> unbind ();
}

bool	FrameBuffer :: isSupported ()
{
	return GLEW_ARB_framebuffer_object != 0;
}

int	FrameBuffer :: maxColorAttachments ()
{
	GLint n;

	glGetIntegerv ( GL_MAX_COLOR_ATTACHMENTS, &n );

	return n;
}

int	FrameBuffer :: maxSize ()
{
	GLint sz;

	glGetIntegerv ( GL_MAX_RENDERBUFFER_SIZE, &sz );

	return sz;
}
