//
// Camera class for OpenGL
//
// Author: Alexey V. Boreskov <steps3d@narod.ru>, <steps3d@gmail.com>
//

#include	<memory.h>
#include	"Camera.h"
#include	"common.h"
#include	"plane.h"
#include	"bbox.h"

#define GLM_ENABLE_EXPERIMENTAL				// for GLM_GTX_euler_angles

#include	<glm/gtx/euler_angles.hpp>
#include	<glm/gtc/matrix_transform.hpp>
#include	<glm/gtc/matrix_inverse.hpp>

Camera :: Camera ( const glm::vec3& p, float yaw, float pitch, float roll,
                   float aFov, float nearZ, float farZ, bool rHanded )
{
	zNear       = nearZ;
	zFar        = farZ;
	pos         = p;
	fov         = aFov;
	rightHanded = rHanded;
	infinite    = false;
	
	setViewSize    ( 640, 480, aFov );	// XXX
	setEulerAngles ( yaw, pitch, roll );
}

/*
Camera :: Camera ( const Camera& camera )
{
	zNear       = camera.zNear;
	zFar        = camera.zFar;
	fov         = camera.fov;
	pos         = camera.pos;
	rightHanded = camera.rightHanded;
	infinite    = camera.infinite;
	width       = camera.width;
	height      = camera.height;
	aspect      = camera.aspect;
	rotation    = camera.rotation;

	computeMatrix ();
}
*/

void    Camera :: setEulerAngles ( float yaw, float pitch, float roll )
{
	rotation = glm::vec3 ( yaw, pitch, roll );

	computeMatrix ();
}

/*
void	Camera :: setOrientation ( const glm::quat& orientation )
{
    glm::mat3  r;

	//orientation.getMatrix ( r );
	r = glm::mat3_cast ( orientation );

	rotation = glm::eulerAngles ( orientation );		//eulerFromMatrix ( r );

    computeMatrix ();
}


glm::quat	Camera :: getOrientation () const
{
	return glm::quat ( rot );		// use existing rotation matrix
}
*/

void	Camera :: setRightHanded  ( bool flag )
{
	if ( flag == rightHanded )
		return;

	rightHanded = flag;

	computeMatrix ();
}

void    Camera :: setFov ( float newFovAngle )
{
	fov = newFovAngle;

	computeMatrix ();
}

void	Camera :: setViewSize ( int theWidth, int theHeight, float theFov )
{
	width  = theWidth;
	height = theHeight;
	fov    = theFov;
	aspect = (float)width / (float) height;

	computeMatrix ();
}

void	Camera :: setZ ( float zn, float zf )
{
	zNear = zn;
	zFar  = zf;

	computeMatrix ();
}

void    Camera :: computeMatrix ()
{
	float yaw   = rotation.x;
	float pitch = rotation.y;
	float roll  = rotation.z;
	
	rot = glm::mat3 ( glm::rotate ( glm::mat4(1), pitch, glm::vec3(1,0,0) ) * 
		  glm::rotate ( glm::mat4(1), yaw,   glm::vec3(0,1,0) ) * 
		  glm::rotate ( glm::mat4(1), roll,  glm::vec3(0,0,1) ) );

	mv   = glm::mat4 ( rot ) * glm::translate ( glm::mat4(1), -pos );
	proj = glm::perspective ( glm::radians(fov), aspect, zNear, zFar );
}

void    Camera :: mirror ( const plane& mirror )
{
	glm::vec3	vDir    = getViewDir ();
	glm::vec3	sideDir = getSideDir ();
	glm::vec3	upDir   = getUpDir   ();
	
	mirror.reflectPos ( pos );
	mirror.reflectDir ( vDir );
	mirror.reflectDir ( upDir );
	mirror.reflectDir ( sideDir );
	
					// now build rotation matrix from vectors
	glm::mat3 r ( sideDir.x, upDir.x, -vDir.x,
	              sideDir.y, upDir.y, -vDir.y,
		          sideDir.z, upDir.z, -vDir.z );
			 
	//rotation    = eulerFromMatrix ( r );	
	
	glm::mat4	r4 ( r );
	
	glm::extractEulerAngleXYZ ( r4, rotation.x, rotation.y, rotation.z );
	rightHanded = !rightHanded;

	computeMatrix ();
}

void	Camera :: getPlanePolyForZ ( float z, glm::vec3 * poly ) const
{
//	float	halfAngle   = 0.5f * M_PI * fov / 180.0f;
//	float	f           = 1 / (float)tan ( halfAngle );
//	glm::vec3	vDir    = getViewDir ();
//	glm::vec3	sideDir = getSideDir ();
//	glm::vec3	upDir   = getUpDir   ();
//	glm::vec3	base ( z * aspect / f, z / f, z );
	glm::mat4	mv   ( getModelView () );
	glm::mat4	proj ( getProjection() );
	glm::mat4	mvInv = glm::inverse ( mv );
/*	
	poly [0] = pos + base.z * vDir - base.x * sideDir - base.y * upDir;
	poly [1] = pos + base.z * vDir - base.x * sideDir + base.y * upDir;
	poly [2] = pos + base.z * vDir + base.x * sideDir + base.y * upDir;
	poly [3] = pos + base.z * vDir + base.x * sideDir - base.y * upDir;
*/
	glm::vec3	base = glm::vec3(1.0f / proj[0][0], 1.0f / proj[1][1], -1);

	poly [0] = glm::vec3 ( mvInv * glm::vec4 ( base * glm::vec3 ( -1, -1, 1 ), 1.0 ) );
	poly [1] = glm::vec3 ( mvInv * glm::vec4 ( base * glm::vec3 ( -1,  1, 1 ), 1.0 ) );
	poly [2] = glm::vec3 ( mvInv * glm::vec4 ( base * glm::vec3 (  1,  1, 1 ), 1.0 ) );
	poly [3] = glm::vec3 ( mvInv * glm::vec4 ( base * glm::vec3 (  1, -1, 1 ), 1.0 ) );
}
bool	Camera::bboxVisible ( const bbox& box ) const
{
	glm::mat4	m ( proj );

	plane	left (  m [3][0] + m [0][0], 
			m [3][1] + m [0][1], 
			m [3][2] + m [0][2],
			m [3][3] + m [0][3] );

	plane	right(  m [3][0] - m [0][0], 
			m [3][1] - m [0][1], 
			m [3][2] - m [0][2],
			m [3][3] - m [0][3] );

	plane	bottom ( m [3][0] + m [1][0],
			 m [3][1] + m [1][1],
			 m [3][2] + m [1][2],
			 m [3][3] + m [1][3] );

	plane	top (    m [3][0] + m [1][0],
			 m [3][1] + m [1][1],
			 m [3][2] + m [1][2],
			 m [3][3] + m [1][3] );

	plane	near (   m [3][0] + m [2][0],
			 m [3][1] + m [2][1],
			 m [3][2] + m [2][2],
			 m [3][3] + m [2][3] );

	plane	far (    m [3][0] - m [2][0],
			 m [3][1] - m [2][1],
			 m [3][2] - m [2][2],
			 m [3][3] - m [2][3] );


	return  (box.classify ( left   ) != IN_BACK) &&
		(box.classify ( right  ) != IN_BACK) &&
		(box.classify ( bottom ) != IN_BACK) &&
		(box.classify ( top    ) != IN_BACK) &&
		(box.classify ( far    ) != IN_BACK) &&
		(box.classify ( near   ) != IN_BACK);
}

