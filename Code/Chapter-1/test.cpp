#include <stdio.h>

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/vec2.hpp>
#include <glm/matrix.hpp>
#include <glm/mat2x2.hpp>
#include <glm/geometric.hpp>

using namespace glm;

float signedArea ( const glm::vec2 p [], int n )
{
    float sum = p [0].x   * (p [1].y - p [n-1].y) + 
                p [n-1].x * (p [0].y - p [n-2].y);
	 
    for ( int i = 1; i < n - 1; i++ )
        sum += p [i].x * (p [i+1].y - p [i-1].y);

    return 0.5f * sum;
}

int main ( int argc, char * argv [] )
{
	glm::vec2 a(1.0f);
	glm::vec2 b = a + 1.0f;
	glm::vec2 c(2.0f,-1.0f);
	glm::vec2 d = a + 2.0f*c;
	glm::vec2 e = -d;
	glm::vec2 f = glm::normalize(e);
	glm::vec2 a2(a.xy());
	glm::vec2 b2(a.xy());
	
	float dot2 = glm::dot(glm::vec2(1), d);
	float len  = glm::length(e);
	float dist = glm::distance(b, c);
	bool  res  = glm::all(glm::equal(a2, b2));
		
	glm::mat2 m(0, 1, 2, 3);
	glm::mat2 n = matrixCompMult(m, m);
	glm::mat2 t = transpose(m);
	glm::mat2 mi = inverse(m);
	
	glm::vec2 u = mi * c + a;

	glm::vec2 p [3];
	
	p [0] = glm::vec2 ( 0, 0 );
	p [1] = glm::vec2 ( 1, 0 );
	p [2] = glm::vec2 ( 0, 1 );
	
	printf ( "%f\n", signedArea ( p, 3 ) );

	return 0;
}
