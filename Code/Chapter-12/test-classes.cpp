#include	"GlutWindow.h"
#include	"Program.h"
#include	"VertexBuffer.h"
#include	"VertexArray.h"

#define	NUM_VERTICES	3
#define	VERTEX_SIZE		(3*sizeof(float))

static const float vertices [] = 
{
	 -1.0f, -1.0f, 0.0f, 
	  0.0f,  1.0f, 0.0f, 
	  1.0f, -1.0f, 0.0f
};

class	TestWindow : public GlutWindow
{
	Program	program;
	VertexArray		vao;
	VertexBuffer	buf;

public:
	TestWindow () : GlutWindow ( 100, 100, 900, 900, "Test" ) 
	{
		if ( !program.loadProgram ( "simple.glsl" ) )
			exit ( "Error building program: %s\n", program.getLog ().c_str () );

		program.bind ();

		vao.create  ();
		vao.bind    ();
		buf.create  ();
		buf.bind    ( GL_ARRAY_BUFFER );
		buf.setData ( NUM_VERTICES * VERTEX_SIZE, vertices, GL_STATIC_DRAW );

		program.setAttrPtr ( "pos", 3, VERTEX_SIZE, nullptr );

		buf.unbind     ();
		vao.unbind     ();
		program.unbind ();
	}

	void	redisplay () override
	{
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		program.bind ();
		vao.bind ();
	
		glDrawArrays ( GL_TRIANGLES, 0, NUM_VERTICES );
	
		vao.unbind ();
		program.unbind ();
	}
};

int main ( int argc, char * argv [] )
{
	GlutWindow::init ( argc, argv );

	TestWindow win;

	return GlutWindow::run ();
}


