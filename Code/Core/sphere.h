#ifndef	__SPHERE__
#define	__SPHERE_

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/vec3.hpp>

class ray;
class bbox;

class sphere
{
	glm::vec3	center;
	float		radius;

public:
	sphere ( const glm::vec3& c, float r ) : center ( c ), radius ( r ) {}
	sphere ( const glm::vec3 * v, int n );
	sphere ( const bbox& box );
	sphere () {}

	const glm::vec3& getCenter () const
	{
		return center;
	}

	float getRadius () const
	{
		return radius;
	}

	bool intersect ( const sphere& s ) const
	{
		return glm::length ( center - s.center ) <= radius + s.radius;
	}
	
	bool intersect ( const ray& r, float& t ) const;

	float volume () const
	{
		return 4.0f * 3.1415926f * radius * radius * radius / 3.0f;
	}

	float area () const
	{
		return 4.0f * 3.1415926f * radius * radius;
	}
};

	
#endif
