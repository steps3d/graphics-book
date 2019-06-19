#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/geometric.hpp>
#include "obb.h"
#include "ray.h"

#define	EPS	0.0001f

bool rayBoxIntersect ( const ray& ray, const obb& box, float& t )
{
	float tNear = -FLT_MAX;
	float tFar  = FLT_MAX;
	float t1, t2;

	for ( int i = 0; i < 3; i++ )
	{
		float vd = glm::dot ( ray.getDir    (), box.n [i] );
		float vo = glm::dot ( ray.getOrigin (), box.n [i] );

		if ( vd > EPS )			// t1 < t2, since d1 [i] < d2 [i]
		{
			t1 = -(vo + box.d2 [i]) / vd;
			t2 = -(vo + box.d1 [i]) / vd;
		}
		else
		if ( vd < -EPS )		// t1 < t2, since d1 [i] < d2 [i]
		{
			t1 = -(vo + box.d1 [i]) / vd;
			t2 = -(vo + box.d2 [i]) / vd;
		}
		else					// abs ( vd ) < Threshold => ray is parallel to slab
		{
			if ( vo < box.d1 [i] || vo > box.d2 [i] )
				return false;
			else
				continue;
		}

		if ( t1 > tNear )
			tNear = t1;

		if ( t2 < tFar )
			if ( ( tFar = t2 ) < EPS )
				return false;

		if ( tNear > tFar )
			return false;
	}

	t = tNear;

	return t > EPS;
}
