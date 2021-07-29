#version 330 core

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform	vec3 eye;		// eye position
uniform vec3 light;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

out vec3  n;
out vec3  v;
out vec3  l;
out vec2  tx;
void main(void)
{
	vec4 	p        = mv * vec4 ( pos, 1.0 );
	vec3	binormal = cross ( normal, tangent );
	
	gl_Position  = proj * p;
	n            = normalize ( nm * normal );
	v            = normalize ( eye - p.xyz );					// vector to the eye
	l            = normalize ( light - p.xyz );
	tx           = texCoord;
}
