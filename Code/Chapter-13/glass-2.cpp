//
// Refraction example 
//
// Author: Alexey V. Boreskov <steps3d@gmail.com>, <steps3d@narod.ru>
//

#include	"GlutRotateWindow.h"
#include	"Program.h"
#include	"BasicMesh.h"
#include	"Texture.h"

class	MyWindow : public GlutRotateWindow
{
	Program	    program, program2;
	BasicMesh * mesh = nullptr;
	BasicMesh * cube = nullptr;
	Texture     cubeMap;

public:
	MyWindow () : GlutRotateWindow ( 100, 50, 900, 900, "Refraction-2" )
	{
		if ( !cubeMap.loadCubemap ( "../../Textures/Snow.dds" ) )
			exit ( "Cannot load cubemap\n" );
		
		if ( !program.loadProgram ( "glass-2.glsl" ) )
			exit ( "Error loading shader: %s\n", program.getLog ().c_str () );
		
		program.bind       ();
		program.setTexture ( "cubeMap", 0 );
		program.unbind     ();

		if ( !program2.loadProgram ( "cube-box.glsl" ) )
			exit ( "Error loading shader: %s\n", program2.getLog ().c_str () );
		
		program2.bind       ();
		program2.setTexture ( "cubeMap", 0 );
		program2.unbind     ();
		
		mesh = createTorus ( 1.5, 3, 30, 30 );
		cube = createBox   ( glm::vec3 ( -7, -7, -7 ), glm::vec3 ( 14, 14, 14 ) );
		eye  = glm::vec3   ( 5, 5, 5 );
	}

	
	void redisplay () override
	{
		glm::mat4	mv = getRotation  ();
		glm::mat3	nm = normalMatrix ( mv );

		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		cubeMap.bind              ( 0 );
		program2.bind             ();
		program2.setUniformMatrix ( "mv",  glm::mat4 ( 1.0f ) );

		cube -> render ();
		
		program2.unbind ();
		
		program.bind             ();
		program.setUniformMatrix ( "mv",  mv );
		program.setUniformMatrix ( "nm",  nm );

		mesh -> render ();

		cubeMap.unbind ();	
		program.unbind ();
	}

	void reshape ( int w, int h ) override
	{
		GlutWindow::reshape ( w, h );
		
		glm::mat4 proj = getProjection (); 

		program.bind             ();
		program.setUniformMatrix ( "proj", proj );
		program.setUniformVector ( "eye",  eye );
		program.unbind           ();  

		program2.bind             ();
		program2.setUniformMatrix ( "proj", proj );
		program2.setUniformVector ( "eye",  eye );
		program2.unbind           ();  
	}
};

int main ( int argc, char * argv [] )
{
	GlutWindow::init( argc, argv );
	
	MyWindow	win;
	
	return GlutWindow::run ();
}
