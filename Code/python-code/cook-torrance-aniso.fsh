#version 330 core

in vec3 v;
in vec3 l;
in vec3 n;
in vec3 t;
in vec3 b;
in vec2 tx;

out vec4 color;

//const float roughness = 0.04;
const float pi        = 3.1415926;
const vec2  sigma     = vec2 ( 0.02, 0.5 );

float sqr ( in float x )
{
	return x * x;
}

void main( void )
{
	vec4  r0   = vec4 ( 1.0, 0.92, 0.23, 1.0);
	vec4  clr  = vec4 ( 0.7, 0.1, 0.1, 1.0 );
	vec3  l2   = normalize ( l );
	vec3  n2   = normalize ( n );
	vec3  t2   = normalize ( t );
	vec3  b2   = normalize ( b );
	vec3  v2   = normalize ( v );
	vec3  h    = normalize ( l2 + v2 );
	float nh   = dot (n2, h);
	float nv   = dot (n2, v2);
	float nl   = dot (n2, l2);
	
					// compute Beckman
	//float r2   = roughness * roughness;
	float nh2  = nh * nh;
	//float ex   = -(1.0 - nh2)/(nh2 * r2);
	//float d    = pow(2.7182818284, ex ) / (r2*nh2*nh2); 

			// compute GGX aniso D
	float d	 = 1 / (pi * sigma.x * sigma.y );

	d /= sqr ( sqr ( dot ( t2, h ) / sigma.x ) + sqr ( dot ( b2, h ) / sigma.y ) + sqr ( dot ( n2, h ) ) );

	vec4  f    = mix(vec4(pow(1.0 - nv, 5.0)), vec4(1.0), r0);
	
					// default G
	float x    = 2.0 * nh / dot(v2, h);
	float g    = min(1.0, min (x * nl, x * nv));
	
					// resulting color
	vec4  ct   = f*(0.25 * d * g / nv);
	float diff = max(nl, 0.0);
	float ks   = 0.5;

	color = /*(vec4(1)-f)**/ diff * clr + ks * ct;
}