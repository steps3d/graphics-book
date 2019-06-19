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
out vec3 l;
out vec3 v;
out vec3 h;

void main(void)
{
	vec4 	p = mv * vec4 ( pos, 1.0 );
	
	gl_Position  = proj * p;
	n            = normalize ( nm * normal );
	v            = normalize ( eye - p.xyz );					// vector to the eye
	l            = normalize ( lightDir );
	h            = normalize ( v + l );
}

-- fragment

#version 330 core

in	vec3 n;
in	vec3 l;
in	vec3 v;
in	vec3 h;

out vec4 color;

void main(void)
{
	const float alpha = 0.7;
	vec3  n2   = normalize ( n );
	vec3  l2   = normalize ( l );
	vec3  v2   = normalize ( v );
	vec3  h2   = normalize ( h );
	float lh   = dot ( l2, h2 );
	float fd90 = 0.5 + 2.0 * lh * lh * alpha;
	float nl   = max ( 0.0, dot ( n2, l2 ) );
	float nv   = max ( 0.0, dot ( n2, v2 ) );
	float diff = (1.0 + (fd90 - 1.0) * pow ( 1.0 - nl, 5.0 ) ) *
                 (1.0 + (fd90 - 1.0) * pow ( 1.0 - nv, 5.0 ) );

	vec4  clr  = vec4 ( 0.7, 0.1, 0.1, 1.0 );

	color = nl * diff * clr; // / 3.1415926;
}

