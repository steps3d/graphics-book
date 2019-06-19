//
// Example of environment mapping
//
// Author: Alexey Boreskov <steps3d@gmail.com>, <steps3d@narod.ru>
//

#include	"GlutRotateWindow.h"
#include	"Program.h"
#include	"BasicMesh.h"
#include	"Texture.h"

class	MeshWindow : public GlutRotateWindow
{
	Program     program;
	BasicMesh * mesh = nullptr;
	Texture     tex;
	
public:
	MeshWindow () : GlutRotateWindow ( 200, 50, 900, 900, "Env. mapped torus" )
	{
		std::string	texName = "../../Textures/Cubemaps/Snow.dds";
	
		if ( !tex.loadCubemap ( texName ) )
			exit ( "Error loading texture %s\n", texName.c_str () );
		
		if ( !program.loadProgram ( "envmapping.glsl" ) )
			exit ( "Error building program: %s\n", program.getLog ().c_str () );
		
		program.bind       ();
		program.setTexture ( "image", 0 );
		
//		mesh = createTorus ( 2, 4, 30, 30  );
		mesh  = createKnot ( 1, 4, 120, 30 );
		eye  = glm::vec3 ( 5, 5, 5 );
	}

	void redisplay () override
	{
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glm::mat4	mv = getRotation  ();
		glm::mat3	nm = normalMatrix ( mv );
	
		tex.bind                 ();
		program.setUniformMatrix ( "mv",  mv );
		program.setUniformMatrix ( "nm",  nm );
		program.setUniformVector ( "eye", eye );

		mesh -> render ();
		
		tex.unbind     ();
	}

	void reshape ( int w, int h ) override
	{
		GlutWindow::reshape ( w, h );
		
		program.setUniformMatrix ( "proj", getProjection () );
	}
};

int main ( int argc, char * argv [] )
{
	GlutWindow::init ( argc, argv );
	
	MeshWindow	win;
	
	return GlutWindow::run ();
}
