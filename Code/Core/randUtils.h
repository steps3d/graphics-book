//
// Simple random generators
//

#pragma once
#ifndef	__RAND_UTILS__
#define	__RAND_UTILS__

#define GLM_FORCE_RADIANS

#ifndef GLM_SWIZZLE
	#define GLM_SWIZZLE
#endif

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include "bbox.h"

float	randUniform  ();
float	randUniform  ( float a, float b );
float	randGauss    ( float m, float sigma );
float	randPoisson  ( float lambda );
glm::vec3	randOnSphere ();
glm::vec3	randInTraingle ( const glm::vec3& a, const glm::vec3& b, const glm::vec3& c );
glm::vec3	getRandomVector ( float length = 1.0f );
glm::vec3	getRandomVector ( const bbox& box );
glm::vec3	getRandomVector ( const glm::vec3& minPt, const glm::vec3& maxPt );

#endif
