-- vertex

#version 330 core

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform	vec3 eye;		// eye position
uniform vec3 lightDir;

layout(location = 0) in vec3 pos;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 binormal;

out vec3 n;
out vec3 v;
out vec3 l;
out vec3 h;
out vec3 t;

void main(void)
{
	vec4 	p = mv * vec4 ( pos, 1.0 );
	
	gl_Position  = proj * p;
	n            = normalize ( nm * normal );
	t            = normalize ( nm * tangent );
	v            = normalize ( eye - p.xyz );					// vector to the eye
	l            = normalize ( lightDir );
	h            = normalize ( v + l );
}

-- fragment
#version 330 core

in vec3 n;
in vec3 v;
in vec3 l;
in vec3 h;
in vec3 t;

out vec4 color;

const float p = 70.0;

void main(void)
{
	vec3  n2   = normalize ( n );
	vec3  t2   = normalize ( t );
	vec3  l2   = normalize ( l );
	vec3  h2   = normalize ( h );
	float lt   = dot(t2, l2);
	float ht   = dot(t2, h2);
	float diff = sqrt ( 1.0 - lt*lt );
	float spec = pow ( 1.0 - ht*ht, p );
	vec4  clr  = vec4 ( 0.7, 0.1, 0.1, 1.0 );
	float ka   = 0.2;
	float kd   = 0.8;
	float ks   = 0.5;

	color = (ka + kd*diff)*clr + ks*vec4(spec);
}
