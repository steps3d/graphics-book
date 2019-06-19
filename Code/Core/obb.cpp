#include	"obb.h"

obb ( const glm::vec3& c, const glm::vec3& e0, const glm::vec3& e1, const glm::vec3& e2 )
{
	center = c;
	e [0]  = e0;
	e [1]  = e1;
	e [2]  = e2;
					// first plane: (p - center, n[0]) = +/- length(e[0])
					// n [0] = normalize ( e [0])
	for ( int i = 0; i < 3; i++ )
	{
		float	len = glm::length ( e [i] );
		
		n  [i] = e [i] / len;
		d1 [i] = glm::dot ( center, n [i] ) - len;
		d2 [i] = glm::dot ( center, n [i] ) + len;
	}
}

/*
obb::obb ( const glm::vec3 * v, int n )
{
	glm::vec3 c ( 0, 0, 0 );

	for ( int i = 0; i < n; i++)
		c += v [i];

	c = c / (float) n;

		// build covariance matrix

		// get its eigenvectors

		// build scale factors for edges
}
*/
