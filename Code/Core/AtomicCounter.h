#pragma once

#include	<GL/glew.h>
#include	"VertexBuffer.h"


class	AtomicCounter
{
	VertexBuffer	buffer;

public:
	AtomicCounter  () = default;
	~AtomicCounter () = default;

	void	create ()
	{
		buffer.create ();
	}

	unsigned getId () const
	{
		return buffer.getId ();
	}

	void	bind ( GLenum target, int index = 0 )
	{
		if ( !buffer.isOk () )
			create ();

		buffer.bindBase ( target, index );
	}

	void	put ( uint32_t value )
	{
		buffer.setData ( sizeof ( value ), &value, GL_STATIC_DRAW );
	}

	uint32_t	get () const
	{
		uint32_t	value;

		buffer.getSubData ( 0, sizeof ( value ), &value );

		return value;
	}

	void	reset ()
	{
		put ( 0 );
	}
};
