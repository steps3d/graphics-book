#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/geometric.hpp>
#include "ray.h"
//#include "sphere.h"

#define	EPS	0.0001f

bool raySphereIntersect ( const ray& ray, const glm::vec3& c, float r, float& t )
{
	glm::vec3 l  = c - ray.getOrigin ();		// direction vector
	float	l2OC = glm::dot ( l, l );		// squared distance
	float	tca  = glm::dot ( l, ray.getDir () );	// closest dist to center
	float	t2hc = r*r - l2OC + tca*tca;
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
