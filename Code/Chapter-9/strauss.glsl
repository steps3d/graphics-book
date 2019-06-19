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
out	vec3  h;

void main(void)
{
	vec4 	p = mv * vec4 ( pos, 1.0 );
	
	gl_Position  = proj * p;
	n            = normalize ( nm * normal );
	v            = normalize ( eye - p.xyz );					// vector to the eye
	l            = normalize ( lightDir );
	h            = normalize ( l + v );	
}

-- fragment

#version 330 core

uniform	float	sm;
uniform	float	mtl;
uniform	float	tr;

in	vec3 n;
in	vec3 v;
in	vec3 l;
in	vec3 h;

out vec4 color;

float	fresnel ( float x, float kf )
{
	float	dx  = x - kf;
	float	d1  = 1.0 - kf;
	float	kf2 = kf * kf;
	
	return (1.0 / (dx * dx) - 1.0 / kf2) / (1.0 / (d1 * d1) - 1.0 / kf2 );
}

float	shadow ( float x, float ks )
{
	float	dx  = x - ks;
	float	d1  = 1.0 - ks;
	float	ks2 = ks * ks;
	
	return (1.0 / (d1*d1) - 1.0 / (dx * dx) ) / (1.0 / (d1 * d1) - 1.0 / ks2 );
}

void main (void)
{
	const vec4	diffColor = vec4 ( 0.7, 0.1, 0.1, 1.0 );
	const vec4	specColor = vec4 ( 1.0, 1.0, 0.0, 1.0 );
	const float	k  = 0.1;
	const float kf = 1.12;
	const float ks = 1.01;
	
	vec3	n2 = normalize ( n );
	vec3	l2 = normalize ( l );
	vec3	v2 = normalize ( v );
	vec3	h2 = reflect   ( l2, n2 );
	float 	nl = dot ( n2, l2 );
	float 	nv = dot ( n2, v2 );
	float	hv = max ( 0.0, -dot ( h2, v2 ) );
	float	f  = fresnel ( nl, kf );
	float	s2 = sm * sm;
 
			// diffuse term
	float	Rd   = ( 1.0 - s2 ) * ( 1.0 - tr );
	vec4	diff = max ( 0.0, nl ) * ( 1.0 - mtl * sm ) * Rd * diffColor;
 
			// inputs into the specular term
	float	r       = (1.0 - tr) - Rd;
	float	j       = f * shadow ( nl, ks ) * shadow ( nv, ks );
	float	reflect = min ( 1.0, r + j * ( r + k ) );
	vec4	C1      = vec4 ( 1.0 );
	vec4 	Cs      = C1 + mtl * (1.0 - f) * (diffColor - C1); 
	vec4	spec    = Cs * reflect * pow ( -hv, 3.0 / (1.0 - sm) );

			// composite the final result, ensuring
	diff = max ( vec4 ( 0.0 ), diff );
	spec = max ( vec4 ( 0.0 ), spec );

	color = diff + spec*specColor;
}
