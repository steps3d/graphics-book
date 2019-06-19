#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp>

glm::quat tangentSpaceToQuat ( const glm::vec3 tangent, 
                               const glm::vec3 binormal,
                               const glm::vec3 normal )
{
	glm::quat	q;

	q.x = normal.y   - binormal.z;
	q.y = tangent.z  - normal.x;
	q.z = binormal.x - tangent.y;
	q.w = 1,.0f + tangent.x + binormal.y + normal.z;

	return glm::normalize ( q );
}

void quatToTangentSpace ( const glm::quat& q, glm::vec3& tangent,
                          glm::vec3& binormal, glm::vec3& normal )
{
	tangent = glm::vec3 ( 1, 0, 0 ) +
                  glm::vec3 ( -2,  2, 2 ) * q.y * glm::vec3 ( q.y, q.z, q.w ) +
                  glm::vec3 ( -2, -2, 2 ) * q.z * glm::vec3 ( q.z, q.w, q.x );

	binormal = glm::vec3 ( 0, 1, 0 ) +
                   glm::vec3 ( 2, -2,  1 ) * q.z * glm::vec3 ( q.w, q.z, q.y ) + 
                   glm::vec3 ( 2, -2, -2 ) * q.x * glm::vec3 ( q.y, q.x, q.w );

	normal = glm::vec3 ( 0, 0, 1 ) + 
                 glm::vec3 ( 2,  2, -2 ) * q.x * glm::vec3 ( q.z, q.w, q.y ) + 
                 glm::vec3 ( -2, 2, -2 ) * q.y * glm::vec3 ( q.w, q.z, q.y );
}
