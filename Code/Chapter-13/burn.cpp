#include	"GlutRotateWindow.h"
#include	"Program.h"
#include	"BasicMesh.h"
#include	"Texture.h"

class	TestWindow : public GlutRotateWindow
{
	Program			program;
	Texture			noiseMap;
	Texture			burnMap;
	glm::vec3		eye;
	glm::vec3		light;
	float			offs  = -1;
	float			scale = 3;
	BasicMesh     * mesh  = nullptr;

public:
	TestWindow () : GlutRotateWindow ( 200, 100, 900, 900, "Burn shader" )
	{
		if ( !noiseMap.load3D ( "../../Textures/noise-3D.dds" ) )
			exit ( "Error loading noise texture\n" );
			
		if ( !burnMap.load2D ( "burngradient.png" ) )
			exit ( "Error loading burn gradient" );
		
		if ( !program.loadProgram ( "burn.glsl" ) )
			exit ( "Error building program: %s\n", program.getLog ().c_str () );
		
		mesh = loadMesh ( "../../Models/teapot.3ds", 0.1f );
		
		if ( mesh == nullptr )
			exit ( "Error loading mesh" );
		
		eye   = glm::vec3 ( 4, 4, 4 );
		light = glm::vec3 ( 7, 7, 7 );
		
		program.bind            ();
		program.setTexture      ( "noiseMap", 0 );
		program.setTexture      ( "burnMap",  1 );
		program.setUniformFloat ( "offs",     offs  );
		program.setUniformFloat ( "scale",    scale );

		burnMap.bind        ( 1 );
		burnMap.setWrapAll  ( GL_CLAMP );
		noiseMap.bind       ( 0 );
		noiseMap.setFilter  ( GL_LINEAR, GL_LINEAR );
	}
	

	void redisplay () override
	{
		glm::mat4	mv = getRotation ();
		glm::mat3	nm = normalMatrix ( mv );
		
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		program.setUniformMatrix ( "mv",  mv );
		program.setUniformMatrix ( "nm",  nm );

		mesh -> render ();
	}

	void reshape ( int w, int h ) override
	{
		GlutRotateWindow::reshape ( w, h );
		
		glm::mat4 proj = glm::perspective ( glm::radians (60.0f), (float)w / (float)h, 0.01f, 20.0f ) * glm::lookAt ( eye, glm::vec3 ( 0, 0, 0 ), glm::vec3 ( 0, 0, 1 ) );
		
		program.setUniformMatrix ( "proj", proj );
		program.setUniformVector ( "eye",   eye );
		program.setUniformVector ( "light", light );
	}

	void	idle () override
	{
		float	t      = getTime ();
		float	angle  = 4 * t;

		light.x = 8*cos ( angle );
		light.y = 8*sin ( 1.4 * angle );
		light.z = 8 + 0.5 * sin ( angle / 3 );

		program.setUniformVector ( "eye",   eye   );
		program.setUniformVector ( "light", light );
		program.setUniformFloat  ( "time",  t );

		glutPostRedisplay ();
	}
};

int main ( int argc, char * argv [] )
{
	GlutWindow::init ( argc, argv );
	
	TestWindow	win;
	
	return GlutWindow::run ();
}
