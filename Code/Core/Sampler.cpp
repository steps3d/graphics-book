//
// ARB_sampler_object C++ wrapper
//

#include	"Sampler.h"

GLenum	Sampler :: getSWrap () const
{
	GLenum sWrap;
	
	glGetSamplerParameteriv ( id, GL_TEXTURE_WRAP_S, (GLint *)&sWrap );

	return sWrap;
}

GLenum	Sampler :: getTWrap () const
{
	GLenum tWrap;
	
	glGetSamplerParameteriv ( id, GL_TEXTURE_WRAP_T, (GLint *)&tWrap );

	return tWrap;
}

GLenum	Sampler :: getRWrap () const
{
	GLenum rWrap;
	
	glGetSamplerParameteriv ( id, GL_TEXTURE_WRAP_R, (GLint *)&rWrap );

	return rWrap;
}

GLenum	Sampler :: getMinFilter () const
{
	GLenum filter;
	
	glGetSamplerParameteriv ( id, GL_TEXTURE_MIN_FILTER, (GLint *)&filter );
	
	return filter;
}

GLenum	Sampler :: getMagFilter () const
{
	GLenum filter;
	
	glGetSamplerParameteriv ( id, GL_TEXTURE_MAG_FILTER, (GLint *)&filter );
	
	return filter;
}

int		Sampler :: getMinLod  () const
{
	int	lod;
	
	glGetSamplerParameteriv ( id, GL_TEXTURE_MIN_LOD, &lod );
	
	return lod;
}

int		Sampler :: getMaxLod  () const
{
	int	lod;
	
	glGetSamplerParameteriv ( id, GL_TEXTURE_MAX_LOD, &lod );
	
	return lod;
}

float	Sampler :: getLodBias () const
{
	float	bias;
	
	glGetSamplerParameterfv ( id, GL_TEXTURE_LOD_BIAS, &bias );
	
	return bias;
}

float	Sampler :: getMaxAnisotropy () const
{
	float	anisotropy;
	
	glGetSamplerParameterfv ( id, GL_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropy );
	
	return anisotropy;
}

GLenum	Sampler :: getTextureCompareMode () const
{
	GLenum	mode;
	
	glGetSamplerParameteriv ( id, GL_TEXTURE_COMPARE_MODE, (GLint *)&mode );
	
	return mode;
}

GLenum	Sampler :: getTextureCompareFunc () const
{
	GLenum func;
	
	glGetSamplerParameteriv ( id, GL_TEXTURE_COMPARE_FUNC, (GLint *)&func );
	
	return func;
}

GLuint Sampler :: activeSampler ()
{
	GLuint	sampler;
	
	glGetIntegerv ( GL_SAMPLER_BINDING, (GLint *)&sampler );
	
	return sampler;
}
