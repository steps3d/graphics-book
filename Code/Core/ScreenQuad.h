//
// Simple class to render screen quad
// Params are passed to vertex shader as a vec4 args ( width, height, sMax, tmax )
//

#pragma once

#include	"VertexArray.h"
#include	"VertexBuffer.h"

class	ScreenQuad
{
	VertexArray	vao;
	VertexBuffer	buf;
	
public:
	ScreenQuad ();
	
	void	render ();
	
protected:
	void	createBuffers ();
};

