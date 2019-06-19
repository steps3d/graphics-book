#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp>
#include <glm/geometric.hpp>

int main ()
{
	glm::vec3 a = glm::vec3(1.0f, 2.0f, 3.0f);
	glm::vec3 b = a.xyz;
	glm::vec3 c(a.xyz);
	glm::vec3 d(a.xyz());
	glm::vec3 e(a.x, a.yz);
	glm::vec3 g (1);
	glm::vec3 h = a + b;
	float	  r = glm::dot ( a, b );
	glm::vec3 p = glm::cross ( b, c );

	glm::vec4 a4 = glm::vec4(1.0f, 2.0f, 3.0f, 4.0f);
	glm::vec4 b4 (a4.xy, a4.z, a4.w);
	
	glm::mat3 l(1.0f);
	glm::vec3 a1 = l * a;
	
	glm::mat3 mm (
			glm::vec3(0.6f, 0.2f, 0.3f), 
			glm::vec3(0.2f, 0.7f, 0.5f), 
			glm::vec3(0.3f, 0.5f, 0.7f));
			
	glm::mat3 mmi = glm::inverse(mm);
	
	glm::mat3 m(0, 1, 2, 3, 4, 5, 6, 7, 8);
	glm::mat3 t = transpose(m);

	glm::quat q1 = glm::angleAxis(glm::pi<float>() * 0.25f, glm::vec3(0, 0, 1));
	glm::quat n  = glm::normalize(q1);
	float     l1 = glm::length(n);
	glm::quat q2 (1.0f, 0.0f, 0.0f, 1.0f);
	float     roll = glm::roll(q1);
	float     pitch = glm::pitch(q1);
	float     yaw = glm::yaw(q1);
	glm::vec3 angles = glm::eulerAngles(q1);
	glm::quat q3 = glm::slerp(q1, q2, 0.2f);
	glm::mat4 rotate = glm::mat4_cast(q1);

	glm::quat pp = glm::log(q2);
	glm::quat rr = glm::exp(pp);

	return 0;
}
