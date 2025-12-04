#version 330 core

layout(location = 0) in vec2 pos;

out vec2 tex;

void main(void)
{
	tex         = pos * 0.5 + vec2 ( 0.5 );
	gl_Position = vec4 ( pos, 1, 1 );
}
