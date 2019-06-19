-- vertex

#version 330 core

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform vec3 light;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

out vec3  n;
out vec3  l;
out vec2  tex;

void main(void)
{
	vec4 	p = mv * vec4 ( pos, 1.0 );
	
	gl_Position  = proj * p;
	n            = normalize ( nm * normal );
	l            = normalize ( light - p.xyz );
	tex          = texCoord;
}

-- fragment

#version 330 core

in vec3 n;
in vec3 l;
in vec2 tex;

out vec4 color;

uniform sampler2D	image;

void main(void)
{
	vec3  n2   = normalize ( n );
	vec3  l2   = normalize ( l );
	float diff = max ( dot ( n2, l2 ), 0.0 );
	vec4  clr  = texture ( image, tex );
	float ka   = 0.2;
	float kd   = 0.8;

	color = (ka + kd*diff) * clr;
}
