//
// Lighting example 
//
// Author: Alexey V. Boreskov <steps3d@gmail.com>, <steps3d@narod.ru>
//

#include	"GlutRotateWindow.h"
#include	"Program.h"
#include	"BasicMesh.h"
#include	"Texture.h"

class	MyWindow : public GlutRotateWindow
{
	Program	    program;
	BasicMesh * mesh = nullptr;
	Texture	    furMap;
	glm::vec3   light = glm::vec3 ( 7, 7, 7 );
	int		numLayers = 20;
	float	    angle = 0;
	float		shadowMin = 0.2;
	float		shadowMax = 0.5;
	
public:
	MyWindow () : GlutRotateWindow ( 200, 50, 900, 900, "Fur rendering" )
	{
		if ( !furMap.load2D ( "../../Textures/fur-map-00.png" ) )
			exit ( "Cannot load fur map texture\n" );
		
		if ( !program.loadProgram ( "fur.glsl" ) )
			exit ( "Error loading shader: %s\n", program.getLog ().c_str () );
		
		program.bind       ();
		program.setTexture ( "furMap", 0 );
		program.unbind     ();

		mesh      = createTorus ( 1.5, 4, 30, 30 );
		eye       = glm::vec3 ( 7, 7, 7 );
	}
	
	void redisplay () override
	{
		glm::mat4	mv        = getRotation  ();
		glm::mat3	nm        = normalMatrix ( mv );
				
		glClear      ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glEnable     ( GL_BLEND );
		glBlendFunc  ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		
		furMap.bind              ();
		program.bind             ();
		program.setUniformMatrix ( "mv",  mv );
		program.setUniformMatrix ( "nm",  nm );
		program.setUniformFloat  ( "time", 4*getTime () );

		for ( int i = 0; i < numLayers; i++ )
		{
			float	t      = (float) i / 16.0f;
//			float	shadow = 0.7*(shadowMin * (1 - t) + shadowMax * t);

			program.setUniformFloat ( "tl", t );

			mesh -> render ();
		}
		
		program.unbind ();
		furMap.unbind  ();
		
		glDisable ( GL_BLEND );
	}

	void reshape ( int w, int h ) override
	{
		GlutWindow::reshape ( w, h );
	   
		program.bind             ();
		program.setUniformMatrix ( "proj",  getProjection () );
		program.setUniformVector ( "eye",   eye );
		program.setUniformVector ( "light", light );
		program.unbind           ();  
	}

	void	idle () override
	{
		angle   = 4 * getTime ();
		light.x = 8*cos ( angle );
		light.y = 8*sin ( 1.4 * angle );
		light.z = 8 + 0.5 * sin ( angle / 3 );

		program.bind             ();
		program.setUniformVector ( "eye",   eye   );
		program.setUniformVector ( "light", light );
		program.unbind           ();

		glutPostRedisplay ();
	}
};

int main ( int argc, char * argv [] )
{
	GlutWindow::init ( argc, argv );
	
	MyWindow	win;
	
	return GlutWindow::run ();
}

