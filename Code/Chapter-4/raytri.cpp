#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/geometric.hpp>
#include "ray.h"

#define	EPS	0.0001f

bool rayTriangleIntersect ( const ray& ray, 
                            const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
			    float& u , float& v, float& t )
{
	glm::vec3 e1 = b - a;
	glm::vec3 e2 = c - a;
	glm::vec3 x  = glm::cross ( ray.getDir (), e2 );
	float     d  = glm::dot ( e1, x );	// determinant of the system

	if ( d > -EPS && d < EPS )
		return false;			// no solution

	float	  f = 1.0 / d;
	glm::vec3 s = ray.getOrigin () - a;

	u =  f * glm::dot ( s, x );

	if ( u < 0 || u > 1 )
		return false;

	glm::vec3 y = glm::cross ( s, e1 );

	v = f * glm::dot ( ray.getDir (), y );

	if ( v < 0 || v > 1 || u + v > 1 )
		return false;

	t = f * glm::dot ( e2, y );
	
	return true;
}
