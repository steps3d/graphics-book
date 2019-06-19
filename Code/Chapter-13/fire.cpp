#include	"GlutWindow.h"
#include	"Program.h"
#include	"Texture.h"
#include	"VertexArray.h"
#include	"VertexBuffer.h"
#include	"randUtils.h"
#include	<glm/gtc/matrix_transform.hpp>

#define	NUM_VERTICES 2000
#define	VERTEX_SIZE	 4*sizeof(float)

class	FireWindow : public GlutWindow
{
	Program		program;
	glm::vec3	eye      = glm::vec3 ( -7, 0, 0 );
	glm::vec4	vertices [NUM_VERTICES];
	Texture		sprite;
	Texture		colorMap;
	VertexArray	vao;
	VertexBuffer	buf;
	
public:
	FireWindow () : GlutWindow ( 200, 100, 900, 900, "Fire" )
	{
		if ( !program.loadProgram ( "fire.glsl" ) )
			exit ( "Error building program: %s\n", program.getLog ().c_str () );
		
		if ( !sprite.load2D ( "Fire.bmp" ) )
			exit ( "Error loading texture\n" );
		
		if ( !colorMap.load2D ( "Flame.tga" ) )
			exit ( "Error loading texture\n" );
		
//		for ( int i = 0; i < NUM_VERTICES; i++ )
		for ( auto& v : vertices )
			v = glm::vec4 ( randUniform (-1,1), randUniform (-1,1), 0.0, randUniform () );
			
		vao.create  ();
		vao.bind    ();
		buf.create  ();
		buf.bind    ( GL_ARRAY_BUFFER );
		buf.setData ( NUM_VERTICES * VERTEX_SIZE, vertices, GL_STATIC_DRAW );

		program.setAttrPtr ( 0, 4, VERTEX_SIZE, nullptr );

		buf.unbind     ();
		vao.unbind     ();
		

		program.bind             ();
		program.setUniformVector ( "eye",      eye );
		program.setTexture       ( "sprite",   0   );
		program.setTexture       ( "colorMap", 1   );
	}

	void redisplay () override
	{
		glm::mat4	mv = glm::mat4 ( 1 );
		
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		sprite.bind              ( 0 );
		colorMap.bind            ( 1 );
		program.bind             ();
		program.setUniformMatrix ( "mv",  mv );
		program.setUniformFloat  ( "time", getTime () );
		vao.bind                 ();
		
		glEnable     ( GL_PROGRAM_POINT_SIZE );
		glEnable     ( GL_BLEND );
		glDisable    ( GL_DEPTH_TEST );
		glBlendFunc  ( GL_ONE, GL_ONE );
		glDrawArrays ( GL_POINTS, 0, NUM_VERTICES );
		glDisable    ( GL_BLEND );
		
		vao.unbind      ();
		program.unbind  ();
		sprite.unbind   ();
		colorMap.unbind ();
	}

	void reshape ( int w, int h ) override
	{
		GlutWindow::reshape ( w, h );
		
		glm::mat4 proj = glm::perspective ( glm::radians(60.0f), getAspect (), 0.01f, 20.0f ) * glm::lookAt ( eye, glm::vec3 ( 0, 0, 0 ), glm::vec3 ( 0, 0, 1 ) );

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
	GlutWindow::init ( argc, argv );
	
	FireWindow	win;
	
	return GlutWindow::run ();
}
