//
// Basic wrapper for OpenGL VBO extension
//
// Author: Alexey V. Boreskov, <steps3d@gmail.com>, <steps3d@narod.ru>
//

#include	"VertexBuffer.h"

VertexBuffer :: VertexBuffer ()
{
	id     = 0;
	ok     = false;
	target = 0;
}

VertexBuffer :: ~VertexBuffer ()
{
	destroy ();
}

void	VertexBuffer :: destroy ()
{
	if ( id != 0 )
		glDeleteBuffers ( 1, &id );

	id     = 0;
	ok     = false;
	target = 0;
}

bool	VertexBuffer :: create ()
{
	glGenBuffersARB ( 1, &id );
	
	return ok = (glIsBuffer ( id ) == GL_TRUE);
}

void	VertexBuffer :: bind ( GLenum theTarget )
{
	glBindBuffer ( target = theTarget, id );
}

void	VertexBuffer :: unbind ()
{
	glBindBuffer ( target, 0 );
}

void	VertexBuffer :: setData ( unsigned size, const void * ptr, GLenum usage )
{
	glBufferData ( target, size, ptr, usage );
}

void	VertexBuffer :: setSubData ( unsigned offs, unsigned size, const void * ptr )
{
	glBufferSubData ( target, offs, size, ptr );
}

void	VertexBuffer :: getSubData ( unsigned offs, unsigned size, void * ptr )
{
	glGetBufferSubData ( target, offs, size, ptr );
}

void	VertexBuffer :: resetData ()
{
	glBufferData ( target, 1, nullptr, GL_STATIC_DRAW );
}

void  * VertexBuffer :: map ( GLenum access )
{
	return glMapBuffer ( target, access );
}

bool	VertexBuffer :: unmap ()
{
	return glUnmapBuffer ( target ) == GL_TRUE;
}

void	VertexBuffer :: setAttrPtr ( int index, int numComponents, GLsizei stride, void * ptr, GLenum type, bool normalized )
{
	glVertexAttribPointer ( index, 					// index
							numComponents, 			// number of values per vertex
							type, 					// type (GL_FLOAT)
							normalized ? GL_TRUE : GL_FALSE,
							stride, 				// stride (offset to next vertex data)
							(const GLvoid*) ptr );
		
	glEnableVertexAttribArray ( index );
}

void	VertexBuffer :: bindBase ( GLenum theTarget, int index )
{
	glBindBufferBase ( target = theTarget, index, id );
}

void	VertexBuffer :: bindRange ( GLenum theTarget, int index, GLintptr offset, GLsizeiptr size )
{
	glBindBufferRange ( target = theTarget, index, id, offset, size );
}


bool	VertexBuffer :: isSupported ()
{
	return GLEW_ARB_vertex_buffer_object != 0;
}
