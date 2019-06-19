#include <glm/geometric.hpp>
#include "sphere.h"
#include "bbox.h"
#include "ray.h"

sphere::sphere ( const glm::vec3 * v, int n )
{
	center = glm::vec3 ( 0 );
	radius = 0;

	for ( int i = 0; i < n; i++ )
		center += v [i];

	center /= (float) n;

	for ( int i = 0; i < n; i++ )
	{
		float r = glm::length ( v [i] - center );

		if ( r > radius )
			radius = r;
	}
}

sphere :: sphere ( const bbox& box )
{
	center = box.getCenter ();
	radius = 0.5f * glm::length ( box.getSize () );
}

bool sphere::intersect ( const ray& r, float& t ) const
{
	glm::vec3 l  = center - r.getOrigin ();		// direction vector
	float	l2OC = glm::dot ( l, l );		// squared distance
	float	tca  = glm::dot ( l, r.getDir () );	// closest dist to center
	float	t2hc = radius*radius - l2OC + tca*tca;
	float	t2;

	if ( t2hc <= 0.0f )
		return false;

	t2hc = sqrtf ( t2hc );

	if ( tca < t2hc )		// we are inside
	{
		t  = tca + t2hc;
		t2 = tca - t2hc;
	}
	else					// we are outside
	{
		t  = tca - t2hc;
		t2 = tca + t2hc;
	}

	if ( fabs ( t ) < EPS )
		t = t2;

	return t > EPS;
}

