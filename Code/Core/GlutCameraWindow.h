//
// Planar reflection example 
//
// Author: Alexey Boreskov <steps3d@gmail.com>, <steps3d@narod.ru>
//

#include	"GlutWindow.h"
#include	"Camera.h"
#include	"common.h"

#include	<glm/matrix.hpp>
#include	<glm/mat3x3.hpp>
#include	<glm/mat4x4.hpp>
#include	<glm/gtc/matrix_transform.hpp>
#include	<glm/gtc/matrix_inverse.hpp>
#include	<glm/gtc/matrix_transform.hpp>

class	GlutCameraWindow : public GlutWindow
{
protected:
	float		yaw       = 0;
	float		pitch     = 0;
	float		roll      = 0;
	float		fov       = 60;
	float		step      = 0.2f;
	float		sideStep  = 0.2f;
	glm::vec3	eye;
	glm::mat4   mv;
	glm::mat4   proj;
	Camera		camera;

public:
	GlutCameraWindow ( int xo, int yo, int w, int h, const char * caption ) : GlutWindow ( xo, yo, w, h, caption ), 
             	                                                              camera     ( glm::vec3(1),  0, 0, 0 )
	{
		camera.setRightHanded ( false );

		eye = glm::vec3 (  0, 0.5, 5 );
	}

	void	setStep ( float v )
	{
		step = v;	
	}
	
	void	setSideStep ( float v )
	{
		sideStep = v;	
	}
	
	glm::mat4 getModelView () const
	{
		glm::vec3 up   = camera.getUpDir   ();
		glm::vec3 view = camera.getViewDir ();
		glm::vec3 pos  = camera.getPos     ();
		
		return glm::lookAt ( pos, pos + view, up );
	}

	const Camera& getCamera () const
	{
		return camera;
	}
	
	glm::mat4 getProjection () const
	{
		return camera.getProjection ();
	}
	
	glm::mat3 normalMatrix ( const glm::mat4& mv ) const
	{
		return glm::inverseTranspose ( glm::mat3 ( mv ) );
	}
	
	void reshape ( int w, int h ) override
	{
		GlutWindow::reshape ( w, h );
		
		camera.setViewSize ( w, h, fov );
	}
	
	void	keyTyped ( unsigned char key, int modifiers, int x, int y ) override
	{
		GlutWindow::keyTyped ( key, modifiers, x, y );

		if ( key == 27 || key == 'q' || key == 'Q' )	//	quit requested
			::exit ( 0 );
			
		if ( key == 'w' || key == 'W' )
			camera.moveBy ( step * camera.getViewDir () );
		else
		if ( key == 'x' || key == 'X' )
			camera.moveBy ( -sideStep * camera.getViewDir () );
		else
		if ( key == 'a' || key == 'A' )
			camera.moveBy ( -step * camera.getSideDir () );
		else
		if ( key == 'd' || key == 'D' )
			camera.moveBy ( sideStep * camera.getSideDir () );
	}

	void	specialKey ( int key, int modifier, int x, int y ) override
	{
		GlutWindow::specialKey ( key, modifier, x, y );

		if ( key == GLUT_KEY_UP )
			pitch += M_PI / 90;
		else
		if ( key == GLUT_KEY_DOWN )
			pitch -= M_PI / 90;
		else
		if ( key == GLUT_KEY_RIGHT )
			yaw += M_PI / 90;
		else
		if ( key == GLUT_KEY_LEFT )
			yaw -= M_PI / 90;

		camera.setEulerAngles ( yaw, pitch, roll );

		glutPostRedisplay ();
	}

	void	mousePassiveMotion ( int x, int y ) override
	{
		static	int	lastX = -1;
		static	int	lastY = -1;

		if ( lastX == -1 )				// not initialized
		{
			lastX = x;
			lastY = y;
		}

		yaw   += (x - lastX) * 0.01f;
		pitch += (y - lastY) * 0.01f;
		lastX  = x;
		lastY  = y;

		camera.setEulerAngles ( yaw, pitch, roll );

		glutPostRedisplay ();
	}

	void mouseWheel ( int wheel, int dir, int x, int y ) override
	{
		eye += 0.5f * glm::vec3 ( dir, dir, dir );

		reshape ( getWidth (), getHeight () );
		
		camera.moveTo ( eye );
		glutPostRedisplay ();
	}
};
