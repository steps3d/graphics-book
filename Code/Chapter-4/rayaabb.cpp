#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/geometric.hpp>
#include "bbox.h"

inline glm::vec3 closestPoint ( const glm::vec3& n, const bbox& box )
{
	return glm::vec3 ( n.x <= 0 ? box.getMaxPoint().x : box.getMinPoint().x,
                           n.y <= 0 ? box.getMaxPoint().y : box.getMinPoint().y,
                           n.z <= 0 ? box.getMaxPoint().z : box.getMinPoint().z );
} 
