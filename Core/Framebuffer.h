//
// C++ wrapper for framebuffer object
//
// Author: Alexey V. Boreskov <steps3d@gmail.com>, <steps3d@narod.ru>
//

#pragma once
#ifndef	__FRAME_BUFFER__
#define	__FRAME_BUFFER__

#include	<GL/glew.h>

class	Texture;
class	TexFormat;

class	FrameBuffer
{
	int		flags;
	int		width;
	int		height;
	GLuint	frameBuffer;					// id of framebuffer object
	Texture * colorBuffer [8];
	GLuint	depthBuffer;
	GLuint	stencilBuffer;					// 
	GLint	saveViewport [4];				// saved viewport setting during bind
	
public:
	FrameBuffer  ();
	~FrameBuffer ();
	
								// disable copying
	FrameBuffer ( const FrameBuffer& ) = delete;
	FrameBuffer& operator = ( const FrameBuffer& ) = delete;
	
	int	getWidth () const
	{
		return width;
	}
	
	int	getHeight () const
	{
		return height;
	}
	
	bool	hasStencil () const
	{
		return stencilBuffer != 0;
	}
	
	bool	hasDepth () const
	{
		return depthBuffer != 0;
	}
	
	Texture * getColorBuffer ( int no = 0 ) const
	{
		return colorBuffer [no];
	}

	unsigned	getDepthBuffer () const
	{
		return depthBuffer;
	}

	void	destroy  ();
	bool	isOk     () const;
	bool	create   (  int theWidth, int theHeight, int theFlags = 0 );
	bool	bind     ();
	bool	bindFace ( int no );
	bool	unbind   ( bool genMipmaps = false,  GLenum target = GL_TEXTURE_2D );
	
	bool	drawBuffers ( int no );		// set number of color buffers to render to
										// must be bound !!!
	
	bool	attachColorTexture   ( Texture * tex, int no = 0 );
	bool	attachCubeMapFace    ( Texture * tex, int face, int no = 0, int mipLevel = 0 );
	bool	attach3DTextureSlice ( Texture * tex, int zOffs, int no = 0 );
	bool	attachDepthTexture   ( GLenum target, unsigned texId );
	bool	attachDepthTexture   ( GLenum target, Texture * tex );

/*	
	bool	detachColorTexture ( GLenum target )
	{
		return attachColorTexture ( target, 0 );
	}
	
	bool	detachDepthTexture ( GLenum target )
	{
		return attachDepthTexture ( target, 0 );
	}
*/
										// create texture for attaching 
	Texture * createColorTexture     ( GLenum format = GL_RGBA, GLenum internalFormat = GL_RGBA8, 
	                                   GLenum clamp = GL_REPEAT, int filter = filterLinear );
										 
	Texture * createColorRectTexture ( GLenum format = GL_RGBA, GLenum internalFormat = GL_RGBA8 );
	
	Texture * createCubemapTexture   ( GLenum format = GL_RGBA, GLenum internalFormat = GL_RGBA8, 
	                                   GLenum clamp = GL_CLAMP_TO_EDGE, int filter = filterLinear );
										 
	Texture * create3DTexture        ( int depth, GLenum format = GL_RGBA, GLenum internalFormat = GL_RGBA8, 
	                                   GLenum clamp = GL_REPEAT, int filter = filterLinear );
										
										// mipmapping support
	void	buildMipmaps ( int no = 0 ) const;
	
	enum								// flags for depth and stencil buffers
	{
		depth16 = 1,					// 16-bit depth buffer
		depth24 = 2,					// 24-bit depth buffer
		depth32 = 4,					// 32-bit depth buffer
		
		stencil1  = 16,					// 1-bit stencil buffer
		stencil4  = 32,					// 4-bit stencil buffer
		stencil8  = 64,					// 8-bit stencil buffer
		stencil16 = 128					// 16-bit stencil buffer
	};
	
	enum								// filter modes
	{
		filterNearest = 0,
		filterLinear  = 1,
		filterMipmap  = 2
	};
	
	static	bool	isSupported         ();
	static	int		maxColorAttachments ();
	static	int		maxSize             ();
};

#endif
