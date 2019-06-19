//
// Simple class to render screen quad
// Params are passed to vertex shader as a vec4 args ( width, height, sMax, tmax )
//

#include	"ScreenQuad.h"

ScreenQuad :: ScreenQuad ()
{

}

void	ScreenQuad :: render ()
{
	if ( vao.getId () == 0 )
		createBuffers ();
		
	vao.bind     ();
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, 4 );
	vao.unbind   ();
}

void	ScreenQuad :: createBuffers ()
{
	const float vertices [] = 
	{
		-1,  1, 0, 1,
		 1,  1, 1, 1,
		-1, -1, 0, 0,
		 1, -1, 1, 0
	};

	vao.create  ();
	vao.bind    ();
	buf.create  ();
	buf.bind    ( GL_ARRAY_BUFFER );
	buf.setData ( 4 * 4 * sizeof ( float ), vertices, GL_STATIC_DRAW );
	
	glVertexAttribPointer ( 0, 						// index
							4, 						// number of values per vertex
							GL_FLOAT, 
							GL_FALSE,
							4 * sizeof ( float),	// stride (offset to next vertex data)
							(const GLvoid*) 0 );
		
	glEnableVertexAttribArray ( 0 );
	
	buf.unbind  ();
	vao.unbind  ();	
}
