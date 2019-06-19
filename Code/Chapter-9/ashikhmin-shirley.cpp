#include	"GlutWindow.h"
#include	"Program.h"
#include	"BasicMesh.h"
#include	<glm/gtc/matrix_transform.hpp>

class	TestWindow : public GlutWindow
{
	Program		program;
	BasicMesh * mesh;
	glm::vec3	eye        = glm::vec3 ( -7, 0, 0 );
	glm::vec3   lightDir   = glm::vec3 ( -1, 1, 1 );
	glm::vec4   lightColor = glm::vec4 ( 2, 2, 2, 1 );
	float 		mx         = 5;
	float 		my         = 70;
	glm::vec4	kd         = glm::vec4 ( 1.0, 0.0, 0.0, 1.0 );
	glm::vec4	ks         = glm::vec4 ( 0.7 );
	float 		r0         = 0.8;
	
public:
	TestWindow () : GlutWindow ( 200, 100, 900, 900, "Ashikhmin-Shirley" )
	{
		if ( !program.loadProgram ( "ashikhmin-shirley.glsl" ) )
			exit ( "Error building program: %s\n", program.getLog ().c_str () );
		
		
		mesh     = createSphere ( glm::vec3 ( 0, 0, 0 ), 2.5, 70, 70 );
//		mesh = createKnot ( 1, 1, 120, 30 );
		
		program.bind ();
		program.setUniformFloat  ( "mx", mx );
		program.setUniformFloat  ( "my", my );
		program.setUniformVector ( "kd", kd );
		program.setUniformVector ( "ks", ks );
		program.setUniformFloat  ( "r0", r0 );
		program.setUniformVector ( "lightDir", lightDir );
		program.setUniformVector ( "lightColor", lightColor );
		program.setUniformVector ( "eye", eye );
	}

	void redisplay ()
	{
		glm::mat4	mv = glm::mat4 ( 1 );
		glm::mat3	nm = glm::mat3 ( 1 );
		
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		program.bind ();
		program.setUniformMatrix ( "mv", mv );
		program.setUniformMatrix ( "nm", nm );

		mesh -> render ();
		
		program.unbind ();
	}

	void reshape ( int w, int h )
	{
		GlutWindow::reshape ( w, h );
		
		glm::mat4 proj = glm::perspective ( glm::radians(60.0f), getAspect(), 0.01f, 20.0f ) * glm::lookAt ( eye, glm::vec3 ( 0, 0, 0 ), glm::vec3 ( 0, 0, 1 ) );

		program.bind ();
		program.setUniformMatrix ( "proj", proj );
		program.unbind ();  
	}

	void mouseWheel ( int wheel, int dir, int x, int y )
	{
		eye += glm::vec3 ( dir, 0, 0 ) * 0.1f;
		
					// since eye value has changed
		reshape ( getWidth(), getHeight() );
		
		glutPostRedisplay ();
	}
	
};

int main ( int argc, char * argv [] )
{
	GlutWindow::init ( argc, argv );
	
	TestWindow	win;
	
	return win.run ();
}
