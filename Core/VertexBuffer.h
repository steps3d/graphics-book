//
// Basic wrapper for OpenGL VBO
//
// Author: Alexey V. Boreskov, <steps3d@gmail.com>, <steps3d@narod.ru>
//

#pragma once
#ifndef	__VERTEX_BUFFER_CLASS__
#define	__VERTEX_BUFFER_CLASS__

#include	<GL/glew.h>
//#include	<string.h>			// for NULL

class	VertexBuffer
{
	GLuint	id;
	GLenum	target;
	bool	ok;
	
								// disable copying
	VertexBuffer ( const VertexBuffer& );
	VertexBuffer& operator = ( const VertexBuffer& );
	
public:
	VertexBuffer  ();
	~VertexBuffer ();

	bool	isOk () const
	{
		return id != 0;
	}
	
	GLuint 	getId () const
	{
		return id;
	}
	
	GLenum	currentTarget () const
	{
		return target;
	}
	
	bool	create  ();
	void	destroy ();
	void	bind    ( GLenum theTarget );
	void	unbind  ();
	
	void	setData    ( unsigned size, const void * ptr, GLenum usage );
	void	setSubData ( unsigned offs, unsigned size, const void * ptr );
	void	getSubData ( unsigned offs, unsigned size, void * ptr );
	void	resetData  ();			// free allocated memory
	void  * map        ( GLenum access );
	bool	unmap      ();

	void	clear ()
	{
		glBufferData ( target, 0, nullptr, 0 /*usage*/ );
	}
	
	void	setAttrPtr ( int index, int numComponents, GLsizei stride, void * ptr, GLenum type = GL_FLOAT, bool normalized = false );
	
								// for transform feedback or uniform buffers only
	void	bindBase  ( GLenum theTarget, int index );
	void	bindRange ( GLenum theTarget, int index, GLintptr offset, GLsizeiptr size );
	
	static	bool	isSupported ();
};

#endif
