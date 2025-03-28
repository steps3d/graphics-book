//
// Camera class for OpenGL
//
// Author: Alexey Boreskov <steps3d@narod.ru>, <steps3d@gmail.com>
//

#pragma once
#ifndef __CAMERA__
#define __CAMERA__

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include "plane.h"

class	quat;
class	bbox;
class	sphere;

class   Camera
{
	glm::mat4	proj;				// projection matrix for the camera (perspective projection)
	glm::mat4	mv;					// model-view matrix (rotation and translation)
	glm::vec3	pos;				// camera position
	glm::vec3	rotation;			// yaw, pitch and roll angles in radians
	glm::mat3	rot;				// rotation matrix corresponding to rotation andles
	bool		rightHanded;		// whether camera is righthanded
	bool		infinite;			// whether it uses zFar equal infinity
	float		fov;				// field of view angle (in degrees)
	float		zNear;				// near clipping z-value
	float		zFar;				// far clipping z-value
	int			width;				// view width in pixels
	int			height;				// view height in pixels
	float		aspect;				// aspect ratio of camera

public:
	Camera ( const glm::vec3& p, float yaw, float pitch, float roll,
             float aFov = 60, float nearZ = 0.1, float farZ = 100, bool rHanded = true );

//	Camera ( const Camera& camera );
	Camera ();

	const glm::vec3&   getPos () const
	{
		return pos;
	}

	glm::vec3	getViewDir () const
	{
		glm::mat3	r ( rot );
		
		return glm::transpose (r) * glm::vec3 ( 0, 0, -1 );
	}

	glm::vec3	getSideDir () const
	{
		glm::mat3	r ( rot );
		
		return glm::transpose (r) * glm::vec3 ( 1, 0, 0 );
	}

	glm::vec3	getUpDir () const
	{
		glm::mat3	r ( rot );
		
		return glm::transpose ( r ) * glm::vec3 ( 0, 1, 0 );
	}

	bool	isRightHanded () const
	{
		return rightHanded;
	}

	float	getZNear () const
	{
		return zNear;
	}

	float	getZFar () const
	{
		return zFar;
	}

	float   getFov () const
	{
		return fov;
	}

	int	getWidth () const
	{
		return width;
	}

	int	getHeight () const
	{
		return height;
	}

	float	getAspect () const
	{
		return aspect;
	}

	const glm::mat4& getProjection () const
	{
		return proj;
	}
	
	const glm::mat4& getModelView () const
	{
		return mv;
	}
	
	void    moveTo ( const glm::vec3& newPos )
	{
		pos = newPos;

		computeMatrix ();
	}

	void    moveBy ( const glm::vec3& delta )
	{
		pos += delta;

		computeMatrix ();					// XXX-since clipping planes must be rebuild
	}
											// set orientation either via Euler angles or
											// via quaternion
	void    setEulerAngles  ( float theYaw, float thePitch, float theRoll );

    										// set viewport parameters
	void	setViewSize     ( int theWidth, int theHeight, float theFov );
	void    setFov          ( float newFovAngle );
	void	setRightHanded  ( bool flag );
	void	setZ		( float zn, float zf );

    										// transform camera
	void    mirror          ( const plane& );

											// return poly (quad) for intersection of plane paraller to zNear plane with given z
	void	getPlanePolyForZ ( float z, glm::vec3 * poly ) const;

        // check most common bounding volumes for visibility
	bool	isVisible ( const bbox& box ) const;
	bool    isVisible ( const sphere& sphere ) const;
	
private:
	void    computeMatrix ();				// compute vectors, transform matrix and build
    										// viewing frustrum
};

#endif
