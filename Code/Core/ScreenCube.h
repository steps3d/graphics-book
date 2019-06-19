//
// Basic class for rendering into a cubemap
// 

#pragma once

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

class	ScreenCube
{
	glm::mat4	proj;		// projection matrix for all sides
	glm::mat4	view [6];	// view matrices for every side of cube

public:
	explicit ScreenCube ( float zNear = 0.1f, float zFar = 10.0f )
	{
		proj     = glm::perspective ( glm::radians ( 90.0f ), 1.0f, zNear, zFar );
		view [0] = glm::lookAt      ( glm::vec3 (0.0f, 0.0f, 0.0f), glm::vec3 ( 1.0f,  0.0f,  0.0f), glm::vec3 (0.0f, -1.0f,  0.0f) );
		view [1] = glm::lookAt      ( glm::vec3 (0.0f, 0.0f, 0.0f), glm::vec3 (-1.0f,  0.0f,  0.0f), glm::vec3 (0.0f, -1.0f,  0.0f) );
		view [2] = glm::lookAt      ( glm::vec3 (0.0f, 0.0f, 0.0f), glm::vec3 ( 0.0f,  1.0f,  0.0f), glm::vec3 (0.0f,  0.0f,  1.0f) );
		view [3] = glm::lookAt      ( glm::vec3 (0.0f, 0.0f, 0.0f), glm::vec3 ( 0.0f, -1.0f,  0.0f), glm::vec3 (0.0f,  0.0f, -1.0f) );
		view [4] = glm::lookAt      ( glm::vec3 (0.0f, 0.0f, 0.0f), glm::vec3 ( 0.0f,  0.0f,  1.0f), glm::vec3 (0.0f, -1.0f,  0.0f) );
		view [5] = glm::lookAt      ( glm::vec3 (0.0f, 0.0f, 0.0f), glm::vec3 ( 0.0f,  0.0f, -1.0f), glm::vec3 (0.0f, -1.0f,  0.0f) );
	}

	const glm::mat4& getProjection () const
	{
		return proj;
	}

	const glm::mat4& getView ( int side ) const
	{
		return view [side];
	}
};

