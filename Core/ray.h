//
// Simple ray class
//

#pragma once
#ifndef	__RAY__
#define	__RAY__

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/vec3.hpp>

class	ray
{
	glm::vec3	org;
	glm::vec3	dir;		// should be normalized
public:
	ray () = default;
	ray ( const glm::vec3& theOrigin, const glm::vec3& theDir, bool normalizeDir = true );

	void	setOrigin ( const glm::vec3& p )
	{
		org = p;
	}
	
	void	setDir ( const glm::vec3& p )
	{
		dir = p;
	}
	
	const glm::vec3&	getOrigin () const
	{
		return org;
	}

	const	glm::vec3&	getDir () const
	{
		return dir;
	}

	glm::vec3	pointAt ( float t ) const
	{
		return org + t * dir;
	}
};

#endif
