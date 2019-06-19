#include	"GlutRotateWindow.h"
#include	"Program.h"
#include	"BasicMesh.h"
#include	"Texture.h"

class	ExplosionWindow : public GlutRotateWindow
{
	Program		program;
	Texture		noise;
	Texture		colorRamp;
	glm::vec3	factor;
	BasicMesh * mesh = nullptr;
	float		period = 5;
	
public:
	ExplosionWindow () : GlutRotateWindow ( 100, 100, 900, 900, "Volumetric explosion" )
	{
		if ( !noise.load3D ( "../../Textures/noise-3D.dds" ) )
			exit ( "Error loading texture\n" );
		
		if ( !colorRamp.load2D ( "explosionramp.png" ) )
			exit ( "Error loading color ramp" );
		
		if ( !program.loadProgram ( "explosion.glsl" ) )
			exit ( "Error building program: %s\n", program.getLog ().c_str () );
		
		program.bind       ();
		program.setTexture ( "noiseMap", 0 );
		program.setTexture ( "colorMap", 1 );
		program.unbind     ();
		
		mesh   = createSphere ( glm::vec3 ( 0 ), 2, 100, 100 );
		eye    = glm::vec3 ( -8, 0, 0 );
	}

	void redisplay () override
	{
		glm::mat4	mv = getRotation  ();
		glm::mat3	nm = normalMatrix ( mv );
		
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		program.bind             ();
		program.setUniformMatrix ( "mv",        mv         );
		program.setUniformMatrix ( "nm",        nm         );
		program.setUniformFloat  ( "time",      getTime () );
		program.setUniformVector ( "factor",    factor     );
		program.setUniformFloat  ( "dispScale", 9          );
		program.setUniformFloat  ( "period",    period     );
		program.setUniformFloat  ( "bias",      0.4f       );

		noise.bind       ( 0 );
		colorRamp.bind   ( 1 );
		mesh -> render   ();
		noise.unbind     ();
		colorRamp.unbind ();
		program.unbind   ();
	}

	void reshape ( int w, int h ) override
	{
		GlutWindow::reshape ( w, h );
		
		program.bind             ();
		program.setUniformMatrix ( "proj", getProjection () );
		program.unbind           ();  
	}

	void mouseWheel ( int wheel, int dir, int x, int y ) override
	{
		eye += glm::vec3 ( dir, dir, dir ) * 0.5f;
		
					// since eye value has changed
		reshape ( getWidth(), getHeight() );
		
		glutPostRedisplay ();
	}
	
	void	idle () override
	{
		float	t = getTime ();
		float	r = sin((t / period)               * 2 * M_PI) * 0.5f + 0.25f;
		float	g = sin((t / period + 0.33333333f) * 2 * M_PI) * 0.5f + 0.25f;
		float	b = sin((t / period + 0.66666667f) * 2 * M_PI) * 0.5f + 0.25f;
		float	correction = 1 / (r + g + b);
		
		r *= correction;
		g *= correction;
		b *= correction;	

		program.bind             ();
		program.setUniformFloat  ( "time",  t );
		program.setUniformVector ( "factor", factor = glm::vec3 ( r, g, b ) );	
		program.unbind           ();
		
		glutPostRedisplay ();
	}
};

int main ( int argc, char * argv [] )
{
	GlutWindow::init ( argc, argv );
	
	ExplosionWindow	win;
	
	return GlutWindow::run ();
}
