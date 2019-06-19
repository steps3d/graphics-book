//
// Texture wrapper class for OpenGL 3.3 and higher
//
// Author: Alexey V. Boreskov
//

#pragma once
#ifndef __TEXTURE_WRAPPER__
#define __TEXTURE_WRAPPER__

#include    <GL/glew.h>
#include    <string>

class	Texture
{
	GLuint		id;
	GLenum		target;
	int		width, height, depth;
	bool		autoMipmaps;	// load mipmaps or create if cannot load
	int		unit;			// texture unit texture is bound to
	
public:
	explicit Texture  ( bool theAutoMipmaps = true );
	~Texture ();
	
								// disable copying
	Texture ( const Texture& ) = delete;
	Texture& operator = ( const Texture& ) = delete;
	
	bool	isOk () const
	{
		return id != 0 && target != 0;
	}
	
	bool	getAutoMipmaps () const
	{
		return autoMipmaps;
	}
	
	int	getId () const
	{
		return id;
	}
	
	int 	getWidth () const
	{
		return width;
	}
	
	int getHeight () const
	{
		return height;
	}
	
	int getDepth () const
	{
		return depth;
	}
	
	GLenum	getTarget () const
	{
		return target;
	}

	
	void	destroy ();

	void	initMips ();
							// create empty texture with given properties
	bool	create1D        ( int theWidth, GLenum fmt, GLenum intFmt );
	bool	create2D        ( int theWidth, int theHeight, GLenum fmt, GLenum intFmt );
	bool	createRectangle ( int theWidth, int theHeight, GLenum fmt, GLenum intFmt );
	bool	create3D        ( int theWidth, int theHeight, int theDepth, GLenum fmt, GLenum intFmt );
	bool	createCubemap   ( int theWidth, GLenum fmt, GLenum intFmt );
	bool	createDepthRect ( int theWidth, int theHeight, GLenum fmt = GL_DEPTH_COMPONENT, GLenum intFmt = GL_DEPTH_COMPONENT );
	//bool	create2DArray   ( int theWidth, int theHeight, int numLayers, GLenum fmt, GLenum intFmt );
	
							// create empty texture arrays
	bool	createArray1D ( int theWidth, int numSlices, GLenum fmt, GLenum intFmt );
	bool	createArray2D ( int theWidth, int theHeight, int numSlices, GLenum fmt, GLenum intFmt );
	
	bool	loadRect       ( const std::string& fileName );
	bool	load2D         ( const std::string& fileName );
	bool	load3D         ( const std::string& fileName );
	bool	loadCubemap    ( const std::string& fileName );
	bool	loadCubemap    ( const char * f1, const char * f2, const char * f3, const char * f4, const char * f5, const char * f6 );
	bool	load2DHdr      ( const std::string& fileName );
	bool	loadCubemapHdr ( const std::string& fileName );
	
							// load cubemap from a list of , or ; separated list of names
	bool	loadCubemapFromNames ( const char * fileNames );
	
	bool	load1DRaw   ( int theWidth, GLenum fmt, GLenum intFmt, GLenum dataType, const void * theData );
	bool	load2DRaw   ( int theWidth, int theHeight, GLenum fmt, GLenum intFmt, GLenum dataType, const void * theData );
	bool	loadRectRaw ( int theWidth, int theHeight, GLenum fmt, GLenum intFmt, GLenum dataType, const void * theData );
	bool	load3DRaw   ( int theWidth, int theHeight, int theDepth, GLenum fmt, GLenum intFmt, GLenum dataType, const void * theData );
	bool	load2DArray ( int width, int height, int numSlices, GLenum theFormat, GLenum theIntFmt, GLenum dataType, const void * theData );
	void	getImage    ( int level, GLenum format, GLenum type, void * pixels );
	
	void	bind   ( int texUnit = 0 );
	void	unbind ();
	
	void	buildMipmaps  ();
	void	setSwizzle     ( GLenum red, GLenum green, GLenum blue, GLenum alpha );
	void	setBaseLevel   ( int level );	// ??? add ivar and getter ???
	void	setMaxLevel    ( int level );

	void	setWrap ( GLenum coord, GLenum mode )
	{
		glTexParameteri ( target, coord, mode );
	}
	
	void	setWrapAll ( GLenum mode )		// set wrapping for all texture coordinates
	{
		glTexParameteri ( target, GL_TEXTURE_WRAP_S, mode );
		glTexParameteri ( target, GL_TEXTURE_WRAP_T, mode );
		glTexParameteri ( target, GL_TEXTURE_WRAP_R, mode );
	}
	
	void	setFilter ( GLenum minFilter, GLenum magFilter )
	{
		glTexParameteri ( target, GL_TEXTURE_MIN_FILTER, minFilter );
		glTexParameteri ( target, GL_TEXTURE_MAG_FILTER, magFilter ) ;
	}

	void	setDefaultFilter ()
	{
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

				// save image as RGB into tga file
				// image must be bound
	bool	saveToFile ( const std::string& fileName, GLenum format = GL_RGB ) const;

	static int	maxSize ();
};

#endif
