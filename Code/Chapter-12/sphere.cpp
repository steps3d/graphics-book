#include	"GlutWindow.h"
#include	"Program.h"
#include	"BasicMesh.h"
#include	"common.h"

#include <glm/gtc/matrix_transform.hpp>

//#include	<stdio.h>
//#include	<stdlib.h>
//#include	<string.h>

class	TestWindow : public GlutWindow
{
	Program		program;
	BasicMesh * mesh;
	glm::vec3	eye;
	glm::vec3   lightDir;
	float		kd  = 0.8f;
	float		ka  = 0.2f;
	
public:
	TestWindow () : GlutWindow ( 200, 100, 900, 900, "Diffuse sphere" )
	{
		if ( !program.loadProgram ( "lambert.glsl" ) )
			exit ( "Error building program: %s\n", program.getLog ().c_str () );
		
		program.bind ();
		
		mesh     = createSphere ( glm::vec3 ( 0, 0, 0 ), 4, 70, 70 );
//		mesh = createKnot ( 1, 3, 120, 30 );
		eye      = glm::vec3 ( 5, 5, 5 );
		lightDir = glm::vec3 ( 1, 1, -1 );

		program.setUniformFloat  ( "kd", kd );
		program.setUniformFloat  ( "ka", ka );
		program.setUniformVector ( "lightDir", lightDir );
	}

	void redisplay () override
	{
		glm::mat4	mv = glm::mat4 ( 1 );
		glm::mat3	nm = glm::mat3 ( 1 );
		
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		program.bind             ();
		program.setUniformMatrix ( "mv", mv );
		program.setUniformMatrix ( "nm",  nm );

		mesh -> render ();
		
		program.unbind ();
	}

	void reshape ( int w, int h ) override
	{
		GlutWindow::reshape ( w, h );
	   
		glm::mat4 proj = glm::perspective ( glm::radians(60.0f), getAspect (), 0.001f, 30.0f ) * glm::lookAt ( eye, glm::vec3 ( 0, 0, 0 ), glm::vec3 ( 0, 1, 0 ) );

		program.bind             ();
		program.setUniformMatrix ( "proj", proj );
		program.unbind           ();  
	}

	void mouseWheel ( int wheel, int dir, int x, int y ) override
	{
		eye += glm::vec3 ( dir, 0, 0 ) * 0.1f;
		
					// since eye value has changed
		reshape ( getWidth(), getHeight() );
		
		glutPostRedisplay ();
	}
	
};

int main ( int argc, char * argv [] )
{
	GlutWindow::init( argc, argv );
	
	TestWindow	win;
	
	return GlutWindow::run ();
}
