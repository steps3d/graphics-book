#version 330 core

uniform mat4 proj;
uniform mat4 mv;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;

out vec2  tx;

void main(void)
{
	vec4 	p = mv * vec4 ( pos, 1.0 );
	
	gl_Position  = proj * p;
	tx           = texCoord * vec2 ( 1, 6 );
}
