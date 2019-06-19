-- vertex

#version 330 core

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform	vec3 eye;		// eye position
uniform vec3 lightDir;

layout(location = 0) in vec3 pos;
layout(location = 2) in vec3 normal;

out vec3  n;
out vec3  v;
out vec3  l;

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

in vec3 v;
in vec3 l;
in vec3 n;

out vec4 color;

const float roughness = 0.09;

void main( void )
{
	vec4  r0   = vec4 ( 1.0, 0.92, 0.23, 1.0);
	vec4  clr  = vec4 ( 0.7, 0.1, 0.1, 1.0 );
	vec3  l2   = normalize ( l );
	vec3  n2   = normalize ( n );
	vec3  v2   = normalize ( v );
	vec3  h    = normalize ( l2 + v2 );
	float nh   = dot (n2, h);
	float nv   = dot (n2, v2);
	float nl   = dot (n2, l2);
	
					// compute Beckman
	float r2   = roughness * roughness;
	float nh2  = nh * nh;
	float ex   = -(1.0 - nh2)/(nh2 * r2);
	float d    = pow(2.7182818284, ex ) / (r2*nh2*nh2); 
	
	vec4  f    = mix(vec4(pow(1.0 - nv, 5.0)), vec4(1.0), r0);
	
					// default G
	float x    = 2.0 * nh / dot(v2, h);
	float g    = min(1.0, min (x * nl, x * nv));
	
					// resulting color
	vec4  ct   = f*(0.25 * d * g / nv);
	float diff = max(nl, 0.0);
	float ks   = 0.5;

	color = diff * clr + ks * ct;
}
