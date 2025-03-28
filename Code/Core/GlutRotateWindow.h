#ifndef	__GLUT_ROTATE_WINDOW__
#define	__GLUT_ROTATE_WINDOW__

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "GlutWindow.h"
#include "common.h"

class GlutRotateWindow : public GlutWindow
{
protected:
	int		mouseOldX = 0;
	int    		mouseOldY = 0;
	glm::vec3	rot = glm::vec3 ( 0.0f );
	glm::vec3	eye = glm::vec3 ( 5, 5, 5 );

public:
	GlutRotateWindow ( int xo, int yo, int w, int h, const char * caption ) : GlutWindow ( xo, yo, w, h, caption ) 
	{
//		mouseOldX = 0;
//		mouseOldY = 0;
//		rot       ;
//		eye       = glm::vec3 ( 5, 5, 5 );
	}

	void	moveEyeTo ( const glm::vec3& p )
	{
		eye = p;

		reshape ( getWidth (), getHeight () );
		
		glutPostRedisplay ();
	}

	void	moveEyeRel ( const glm::vec3& delta )
	{
		moveEyeTo ( eye + delta );
	}

	glm::mat4 getRotation () const
	{
		glm::mat4 m ( 1 );

		m = glm::rotate(m, glm::radians(-rot.z), glm::vec3(0, 0, 1));
		m = glm::rotate(m, glm::radians(rot.y),  glm::vec3(0, 1, 0));
		m = glm::rotate(m, glm::radians(rot.x),  glm::vec3(1, 0, 0));

		return glm::lookAt ( eye, glm::vec3 ( 0.0f, 1.0f, 0.0f ), glm::vec3 ( 0, 0, 1 ) ) * m;
	}

	glm::mat3 normalMatrix ( const glm::mat4& mv ) const
	{
		return glm::inverseTranspose ( glm::mat3 ( mv ) );
	}
	
	glm::mat4 getProjection ( float fovDegrees = 60.0f, float zNear = 0.1f, float zFar = 100.0f ) const
	{
		return glm::perspective ( glm::radians( fovDegrees ), (float)getWidth () / (float)getHeight (), zNear, zFar ); 
	}
	
	void	mouseMotion ( int x, int y ) override
	{
		rot.x -= ((mouseOldY - y) * 180.0f) / 200.0f;
		rot.z -= ((mouseOldX - x) * 180.0f) / 200.0f;
		rot.y  = 0;

		if ( rot.z > 360 )
			rot.z -= 360;

		if ( rot.z < -360 )
			rot.z += 360;

		if ( rot.y > 360 )
			rot.y -= 360;

		if ( rot.y < -360 )
			rot.y += 360;

		mouseOldX = x;
		mouseOldY = y;

		glutPostRedisplay ();
	}
	
	void	mouseClick ( int button, int state, int modifiers, int x, int y ) override
	{
		if ( state == GLUT_DOWN )
		{
			mouseOldX = x;
			mouseOldY = y;
		}
	}
};

#endif
