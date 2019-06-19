#include	"GlutRotateWindow.h"
#include	"Program.h"
#include	"BasicMesh.h"
#include	"Texture.h"

class	MeshWindow : public GlutRotateWindow
{
	Program		program;
	Texture		bumpMap;
	Texture		colorMap;
	BasicMesh * mesh = nullptr;
	glm::vec3   light = glm::vec3  ( 9, 9, 9 );
	
public:
	MeshWindow () : GlutRotateWindow ( 100, 50, 900, 900, "Bumpmapping" )
	{
		if ( !program.loadProgram ( "bumpmapping.glsl" ) )
			exit ( "Error building program: %s\n", program.getLog ().c_str () );
		
		if ( !bumpMap.load2D  ( "../../Textures/Bumpmaps/FieldStone-DOT3.tga" ) )
			exit ( "Error loaing bumpmap\n" );
			
		if ( !colorMap.load2D ( "../../Textures/Fieldstone.dds" ) )
			exit ( "Error loading decal map\n" );
		
		program.bind       ();
		program.setTexture ( "colorMap", 0 );
		program.setTexture ( "bumpMap",  1 );
		
		mesh  = createKnot ( 1, 4, 120, 30 );
		eye   = glm::vec3  ( -8, 0, 0 );
	}

	void redisplay () override
	{
		const glm::mat4	mv = getRotation  ();
		const glm::mat3	nm = normalMatrix ( mv );
		
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		colorMap.bind            ( 0 );
		bumpMap.bind             ( 1 );
		program.setUniformMatrix ( "mv", mv );
		program.setUniformMatrix ( "nm", nm );

		mesh -> render ();
	}

	void reshape ( int w, int h ) override
	{
		GlutWindow::reshape ( w, h );
		
		program.setUniformMatrix ( "proj", getProjection () );
	}

	void mouseWheel ( int wheel, int dir, int x, int y ) override
	{
		eye += glm::vec3 ( dir, 0, 0 ) * 0.5f;
		
					// since eye value has changed
		reshape ( getWidth(), getHeight() );
		
		glutPostRedisplay ();
	}
	
	void	idle () override
	{
		const float	angle = 4 * getTime ();
		
		light.x = 9*cos ( angle );
		light.y = 9*sin ( 1.4 * angle );
		light.z = 9 + 0.5 * sin ( angle / 3 );

//		program.bind             ();
		program.setUniformVector ( "light", light );
		program.setUniformVector ( "eye",   eye );
//		program.unbind           ();
		
		glutPostRedisplay ();
	}
};

int main ( int argc, char * argv [] )
{
	GlutWindow::init ( argc, argv );
	
	MeshWindow	win;
	
	return GlutWindow::run ();
}
