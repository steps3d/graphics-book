//
// EMBM example 
//
// Author: Alexey Boreskov <steps3d@gmail.com>, <steps3d@narod.ru>
//

#include	"GlutRotateWindow.h"
#include	"Program.h"
#include	"BasicMesh.h"
#include	"Texture.h"

class	MeshWindow : public GlutRotateWindow
{
	Program	    program;
	BasicMesh * mesh = nullptr;
	Texture     bumpMap;
	Texture     cubeMap;
	
public:
	MeshWindow () : GlutRotateWindow ( 100, 100, 900, 900, "EMBM torus" )
	{
		std::string	texName = "../../Textures/Cubemaps/Snow.dds";
	
		if ( !cubeMap.loadCubemap ( texName ) )
			exit ( "Error loading texture %s\n", texName.c_str () );
														
		if ( !bumpMap.load2D ( "../../Textures/Bumpmaps/FieldStone-DOT3.tga" ) )
			exit ( "Error loading bump map\n");
		
		if ( !program.loadProgram ( "embm.glsl" ) )
			exit ( "Error building program: %s\n", program.getLog ().c_str () );
		
		program.bind       ();
		program.setTexture ( "bumpMap", 0 );
		program.setTexture ( "cubeMap", 1 );
		program.unbind     ();

		mesh  = createKnot ( 1, 3, 120, 30 );
		eye   = glm::vec3  ( 4 );
	}

	void redisplay () override
	{
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glm::mat4	mv = getRotation  ();
		glm::mat3	nm = normalMatrix ( mv );

		program.bind ();
		program.setUniformMatrix ( "mv",  mv );
		program.setUniformMatrix ( "nm",  nm );

		cubeMap.bind ( 1 );
		bumpMap.bind ( 0 );
		
		mesh -> render ();

		cubeMap.unbind ();
		bumpMap.unbind ();
		program.unbind ();
	}

	void reshape ( int w, int h ) override
	{
		GlutWindow::reshape ( w, h );
		
		program.bind             ();
		program.setUniformMatrix ( "proj",  getProjection () );
		program.setUniformVector ( "eye",   eye );
		program.unbind           ();  
	}
};

int main ( int argc, char * argv [] )
{
	GlutWindow::init ( argc, argv );
	
	MeshWindow	win;
	
	return GlutWindow::run ();
}
