#version 330 core

layout(location = 0) in vec2 pos;

out vec4 c;

void main(void)
{
	c           = vec4 ( pos * 0.5 + vec2 ( 0.5 ), 0, 1 );
	gl_Position = vec4 ( pos, 1, 1 );
}
