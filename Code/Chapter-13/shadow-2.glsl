-- vertex

#version 330 core

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform vec3 light;
uniform mat4 shadowMat;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

out vec3  n;
out vec3  l;
out vec2  tex;
out vec4  shadowPos;

void main(void)
{
	vec4 	p = mv * vec4 ( pos, 1.0 );
	
	gl_Position  = proj * p;
	n            = normalize ( nm * normal );
	l            = normalize ( light - p.xyz );
	tex          = texCoord;
	shadowPos    = shadowMat * p;
}

-- fragment

#version 330 core

in  vec3 n;
in  vec3 l;
in  vec2 tex;
in  vec4 shadowPos;
out vec4 color;

uniform sampler2D image;
uniform sampler2DShadow shadowMap;

//const float bias = -0.0001;

void main(void)
{
	vec3  n2   = normalize ( n );
	vec3  l2   = normalize ( l );
	float diff = max ( dot ( n2, l2 ), 0.0 );
	vec4  clr  = texture ( image, tex );
	float ka   = 0.2;
	float kd   = 0.8;

	vec3 p = 0.5 * (shadowPos.xyz / shadowPos.w) + vec3 ( 0.5 );
	
	p.z -= 0.00003;
	
	float	shadow = 1.0 - texture ( shadowMap, p );
	
	color = ka*clr + kd*diff * clr * (1.0 - shadow);
}
