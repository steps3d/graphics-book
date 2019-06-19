-- vertex

#version 330 core

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform	vec3 eye;		// eye position
uniform vec3 lightDir;

layout(location = 0) in vec3 pos;
layout(location = 2) in vec3 normal;

out vec3 n;
out vec3 v;
out vec3 l;

void main(void)
{
	vec4 	p = mv * vec4 ( pos, 1.0 );
	
	gl_Position  = proj * p;
	n            = normalize ( nm * normal );
	v            = normalize ( eye - p.xyz );					// vector to the eye
	l            = normalize ( lightDir );
}

-- fragment
#version 330 core

in	vec3 n;
in	vec3 v;
in	vec3 l;

out vec4 color;

const float k = 0.3;

void main(void)
{

	vec3  n2   = normalize ( n );
	vec3  l2   = normalize ( l );
	vec3  v2   = normalize ( v );
	float nl   = max ( 0.0, dot ( n2, l2 ) );
	float nv   = max ( 0.0, dot ( n2, v2 ) );
	vec4  clr  = vec4 ( 0.7, 0.1, 0.1, 1.0 );
	float diff = pow ( nl, 1.0 + k ) * pow ( 1.0 - nv, 1.0 - k );
	float ka   = 0.2;
	float kd   = 0.8;

	color = (ka + kd*diff)*clr;
}
