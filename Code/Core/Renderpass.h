#pragma once

#include	<vector>
#include	"Program.h"
#include	"Texture.h"
#include	"Framebuffer.h"

template <class C>
class	with
{
	C * ptr;

public:
	with ( C * p ) : ptr ( p ) 
	{
		ptr->bind ();
	}

	~with ()
	{
		ptr->unbind ();
	}

	operator bool () const
	{
		return true;
	}
};

class	Renderpass
{
	GLbitfield				     clear   = GL_COLOR_BUFFER_BIT;
	Program				       * program = nullptr;
	FrameBuffer			       * target  = nullptr;
	std::vector<std::pair<Texture *, int>>	textures;

public:
	Renderpass () = default;
	Renderpass ( Program& p ) : program ( &p ) {}
	~Renderpass () = default;

	Renderpass&	setProgram ( Program& p )
	{
		program = &p;

		return *this;
	}

	Renderpass&	setTarget ( FrameBuffer& fb )
	{
		target = &fb;

		return *this;
	}

	Renderpass&	setClear ( GLbitfield mask )
	{
		clear = mask;

		return *this;
	}

	Renderpass&	setTexture ( Texture * tex, int unit = 0 )
	{
		textures.push_back ( std::make_pair ( tex, unit ) );
		
		return *this;
	}
	
	Renderpass&	setTexture ( std::initializer_list<std::pair<Texture *, int>> texs )
	{
		for ( auto& b : texs )
			textures.push_back ( std::make_pair ( b.first, b.second ) );
		
		return *this;
	}
	
/*
	template <typename C>
	void	render ( C * renderable )
	{
		if ( target )
			target -> bind ();

		if ( clear )
			glClear ( clear );

		for ( auto& p : textures )
			p.first -> bind ( p.second );

		program    -> bind   ();
		renderable -> render ();
		program    -> unbind ();

		if ( target )
			target -> unbind ();
	}

	template <typename C>
	void	render ( C& renderable )
	{
		if ( target )
			target -> bind ();

		if ( clear )
			glClear ( clear );

		for ( auto& p : textures )
			p.first -> bind ( p.second );

		program    -> bind   ();
		renderable.render    ();
		program    -> unbind ();

		if ( target )
			target -> unbind ();
	}
*/
	template <typename C>
	void	render ( C foo )
	{
		if ( target )
			target -> bind ();

		if ( clear )
			glClear ( clear );

		for ( auto& p : textures )
			p.first -> bind ( p.second );

		program    -> bind   ();
		foo                  ();
		program    -> unbind ();

		if ( target )
			target -> unbind ();
	}
};
