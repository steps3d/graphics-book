#include	"GlutWindow.h"
#include	"Program.h"
#include	"BasicMesh.h"
#include	<glm/gtc/matrix_transform.hpp>

class	TestWindow : public GlutWindow
{
	Program		program;
	BasicMesh * mesh;
	glm::vec3	eye      = glm::vec3 ( -7, 0, 0 );
	glm::vec3   lightDir = glm::vec3 ( -1, 1, 1 );
	float		sm       = 0.8;
	float		mtl      = 0.0;
	float		tr       = 0.0;
	
public:
	TestWindow () : GlutWindow ( 200, 100, 900, 900, "Strauss" )
	{
		if ( !program.loadProgram ( "strauss.glsl" ) )
			exit ( "Error building program: %s\n", program.getLog ().c_str () );
				
		mesh     = createSphere ( glm::vec3 ( 0, 0, 0 ), 2.5, 70, 70 );
//		mesh = createKnot ( 1, 1, 120, 30 );

		program.bind ();
		program.setUniformFloat  ( "sm", sm );
		program.setUniformFloat  ( "mtl", mtl );
		program.setUniformFloat  ( "tr", tr );
		program.setUniformVector ( "lightDir", lightDir );
		program.setUniformVector ( "eye", eye );
	}

	void redisplay ()
	{
		glm::mat4	mv = glm::mat4 ( 1 );
		glm::mat3	nm = glm::mat3 ( 1 );
		
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		program.bind ();
		program.setUniformMatrix ( "mv", mv );
		program.setUniformMatrix ( "nm",  nm );

		mesh -> render ();
		
		program.unbind ();
	}

	void reshape ( int w, int h )
	{
		GlutWindow::reshape ( w, h );
		
		glm::mat4 proj = glm::perspective ( glm::radians(60.0f), (float)w / (float)h, 0.01f, 20.0f ) * glm::lookAt ( eye, glm::vec3 ( 0, 0, 0 ), glm::vec3 ( 0, 0, 1 ) );
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
