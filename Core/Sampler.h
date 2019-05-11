//
// ARB_sampler_object C++ wrapper
//

#pragma once
#ifndef	__SAMPLER_OBJECT__
#define	__SAMPLER_OBJECT__

#include	<GL/glew.h>

class	Sampler
{
	GLuint	id;
	
								// disable copying
	Sampler ( const Sampler& );
	Sampler& operator = ( const Sampler& );
	
public:
	Sampler  ()
	{
		id = 0;
	}
	
	~Sampler ()
	{
		destroy ();
	}
	
	bool	isOk () const
	{
		return id != 0 && glIsSampler ( id );
	}
	
	GLuint	getId () const
	{
		return id;
	}
	
	void create ()
	{
		glGenSamplers ( 1, &id );
	}

	void destroy ()
	{
		glDeleteSamplers ( 1, &id );

		id = 0;
	}

	void bind ( int unit )
	{
		glBindSampler ( unit, id );
	}
	
	void	unbind ( int unit )
	{
		glBindSampler ( unit, 0 );
	}
	
	void	setWrap ( GLenum sWrap, GLenum tWrap, GLenum rWrap )
	{
		glSamplerParameteri ( id, GL_TEXTURE_WRAP_S, sWrap );
		glSamplerParameteri ( id, GL_TEXTURE_WRAP_T, tWrap );
		glSamplerParameteri ( id, GL_TEXTURE_WRAP_R, rWrap );
	}
	
	void	setMinFilter ( GLenum minFilter )
	{
		glSamplerParameteri ( id, GL_TEXTURE_MIN_FILTER, minFilter );
	}
	
	void	setMagFilter ( GLenum magFilter )
	{
		glSamplerParameteri ( id, GL_TEXTURE_MAG_FILTER, magFilter );
	}
	
	void	setMinLod  ( int minLod )
	{
		glSamplerParameteri ( id, GL_TEXTURE_MIN_LOD, minLod );
	}
	
	void	setMaxLod  ( int maxLod )
	{
		glSamplerParameteri ( id, GL_TEXTURE_MAX_LOD, maxLod );
	}
	
	void	setLodBias ( float bias )
	{
		glSamplerParameterf ( id, GL_TEXTURE_LOD_BIAS, bias );
	}
	
	void	setMaxAnisotropy ( float anisotropy )
	{
		glSamplerParameterf ( id, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy );
	}
	
	void	setTextureCompare ( GLenum mode, GLenum func )
	{
		glSamplerParameteri ( id, GL_TEXTURE_COMPARE_MODE, mode );
		glSamplerParameteri ( id, GL_TEXTURE_COMPARE_FUNC, func );
	}
	
	
	GLenum	getSWrap         () const;
	GLenum	getTWrap         () const;
	GLenum	getRWrap         () const;
	GLenum	getMinFilter     () const;
	GLenum	getMagFilter     () const;
	int		getMinLod        () const;
	int		getMaxLod        () const;
	float	getLodBias       () const;
	
	float	getMaxAnisotropy      () const;
	GLenum	getTextureCompareMode () const;
	GLenum	getTextureCompareFunc () const;

	static GLuint activeSampler ();			// return sampler bound to current texture unit
};

#endif
