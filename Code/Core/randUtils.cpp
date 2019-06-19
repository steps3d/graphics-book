//
// Simple random generators
//

#include	<glm/vec3.hpp>
#include	<glm/geometric.hpp>
#include	<math.h>
#include	<stdlib.h>
#include	"randUtils.h"
#include	"common.h"

#ifndef	M_PI								// since not all compilers define it
	#define M_PI        3.14159265358979323846f
#endif

float	randUniform ()
{
	return rand () / (float) RAND_MAX;
}

float	randUniform ( float a, float b )
{
	return a + randUniform () * (b - a);
}

float	randGauss ( float m, float sigma )
{
  // from http://www.taygeta.com/random/gaussian.html
  // Algorithm by Dr. Everett (Skip) Carter, Jr.

	float x1, x2, w;
 
	do 
	{
		x1 = randUniform ( -1, 1 );
		x2 = randUniform ( -1, 1 );
		w = x1 * x1 + x2 * x2;
    } while ( w >= 1.0f );

    w = sqrtf( (-2.0f * log ( w ) ) / w );

    return x1 * w * sigma + m;
}

float	randPoisson ( float lambda )
{
	float l = exp (-lambda );
	float p = 1.0f;
	int   k = 0;

	do 
	{
		k++;
		p *= randUniform ();
  	} while ( p > l );

	return (float)(k - 1);
}

glm::vec3	randOnSphere ()
{
	float	t = randUniform ( 0, 2*M_PI );		// случайное число, равномерно распределенное в [0,2*pi]
	float	z = randUniform ( -1, 1 );  		// случайное число, равномерно распределенное в [0,1]
	float	r = sqrt ( 1 - z*z );

	return glm::vec3 ( r * cos ( t ), r * sin ( t ), z );

}

glm::vec3	randInTraingle ( const glm::vec3& a, const glm::vec3& b, const glm::vec3& c )
{
	float	u = randUniform ( 0, 1 );
	float	v = randUniform ( 0, 1 );
	float	w;

	if ( u + v > 1 )
	{
		u = 1 - u;
    		v = 1 - v;
	}

	w = 1 - u - v;

	return u*a + v*b + w*c;
}

glm::vec3	getRandomVector ( float len )
{
	for ( ; ; )
	{
		glm::vec3 v ( randUniform (-1,1), randUniform (-1,1), randUniform (-1,1) );
		float	l = glm::length ( v );

		if ( l < EPS )
			continue;

		v *= len / l;

		return v;
	}
}

glm::vec3	getRandomVector ( const bbox& box )
{
	glm::vec3	v;

	v.x = randUniform ( box.getMinPoint ().x, box.getMaxPoint ().x );
	v.y = randUniform ( box.getMinPoint ().y, box.getMaxPoint ().y );
	v.z = randUniform ( box.getMinPoint ().z, box.getMaxPoint ().z );

	return v;
}

glm::vec3	getRandomVector ( const glm::vec3& minPoint, const glm::vec3& maxPoint )
{
	glm::vec3	v;

	v.x = randUniform ( minPoint.x, maxPoint.x );
	v.y = randUniform ( minPoint.y, maxPoint.y );
	v.z = randUniform ( minPoint.z, maxPoint.z );

	return v;
}
