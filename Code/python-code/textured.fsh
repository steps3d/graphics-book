#version 330 core

in vec2 tx;

out vec4 color;

uniform sampler2D image;

void main( void )
{
	color = texture ( image, tx );
}
