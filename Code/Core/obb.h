#ifndef	__OBB__
#define	__OBB__

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/vec3.hpp>

class ray;

class obb
{
public:
	glm::vec3	center;		// center of OBB
	glm::vec3	e  [3];		// edge direction and size
	glm::vec3	n  [3];		// normals to faces
	float		d1 [3];		// plane coefficients for the slab
	float 		d2 [3];

	obb ( const glm::vec3& c, const glm::vec3& e0, const glm::vec3& e1, const glm::vec3& e2 );
	//obb ( const glm::vec3 * v, int n );

	bool intersect ( const obb& other ) const;
	bool intersect ( const ray& r, float& t1, float& t2 ) const;
};

#endif
