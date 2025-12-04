#version 330 core

uniform sampler2D image;

in vec2 tex;
out vec4 color;

void main(void)
{
	color = texture ( image, tex );
}
