//
// Wrapper for kDop bounding volume
//
// Author: Alexey V. Boreskov, <steps3d@narod.ru>, <steps3s@gmail.com>
//

#pragma once
#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/vec3.hpp>

class	plane;

class kdopBase
{
public:
	enum 
	{
		maxCount = 20
	};

private:
	glm::vec3	n [maxCount];
	int			count;

public:
	kdopBase ( const glm::vec3 *, int );

	int getCount () const
	{
		return count;
	}

	const glm::vec3& getNormal ( int i ) const
	{
		return n [i];
	}
};

class kdop
{
	kdopBase * base;
	float      dMin [kdopBase::maxCount];
	float      dMax [kdopBase::maxCount];

public:
	kdop ( kdopBase * theBase );
	kdop ( const kdop& );

	void addVertices ( const glm::vec3 * v, int n );
	void merge       ( const kdop& );

	bool intersect ( const kdop& ) const;
	int  classify  ( const plane& ) const;
	bool intersect ( const ray& r, float& t1, float& t2 ) const;
};
