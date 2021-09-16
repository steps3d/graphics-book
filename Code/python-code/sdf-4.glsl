-- vertex

#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;

out vec3 p;

void main(void)
{
	p           = vec3 ( 1.4*pos.xy, 0.0 );
	gl_Position = vec4 ( pos.xy, 0.0, 1.0 );
}

-- fragment

#version 330 core

#define	EPS				1.0e-5

uniform mat3 	  mv;
uniform vec3 	  eye;
uniform sampler3D noiseMap;

in  vec3 p;
out vec4 color;

float hash ( float n ) 
{ 
	return fract(sin(n) * 1e4); 
}

float hash ( vec2 p )
{
	return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); 
}

float noise ( float x )
{
	float i = floor(x);
	float f = fract(x);
	float u = f * f * (3.0 - 2.0 * f);
	
	return mix(hash(i), hash(i + 1.0), u);
}

float noise ( vec2 x )
{
	vec2 i = floor(x);
	vec2 f = fract(x);

		// Four corners in 2D of a tile
	float a = hash(i);
	float b = hash(i + vec2(1.0, 0.0));
	float c = hash(i + vec2(0.0, 1.0));
	float d = hash(i + vec2(1.0, 1.0));

	// Simple 2D lerp using smoothstep envelope between the values.
	// return vec3(mix(mix(a, b, smoothstep(0.0, 1.0, f.x)),
	//			mix(c, d, smoothstep(0.0, 1.0, f.x)),
	//			smoothstep(0.0, 1.0, f.y)));

	// Same code, with the clamps in smoothstep and common subexpressions
	// optimized away.
	vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

// This one has non-ideal tiling properties that I'm still tuning
float noise ( vec3 x )
{
	const vec3 step = vec3(110, 241, 171);

	vec3 i = floor(x);
	vec3 f = fract(x);
 
	// For performance, compute the base input to a 1D hash from the integer part of the argument and the 
	// incremental change to the 1D based on the 3D -> 1D wrapping
    float n = dot(i, step);

	vec3 u = f * f * (3.0 - 2.0 * f);
	return mix(mix(mix( hash(n + dot(step, vec3(0, 0, 0))), hash(n + dot(step, vec3(1, 0, 0))), u.x),
                   mix( hash(n + dot(step, vec3(0, 1, 0))), hash(n + dot(step, vec3(1, 1, 0))), u.x), u.y),
               mix(mix( hash(n + dot(step, vec3(0, 0, 1))), hash(n + dot(step, vec3(1, 0, 1))), u.x),
                   mix( hash(n + dot(step, vec3(0, 1, 1))), hash(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z);
}

float turbulence ( in vec3 t )
{
  return 2.0*abs ( noise ( t     ) ) + 
         1.0*abs ( noise ( 2.0*t ) ) +
         0.5*abs ( noise ( 4.0*t ) );
}

float dE ( vec3 p )
{
	p *= mv;
	
	return length ( p ) - 1.0  + 0.1 * turbulence(2*p);
}

		// Box
/*		
vec3 dE ( vec3 p )
{
	vec3 b = vec3 ( 0.3 );
	
	p *= mv;
	
	return vec3 ( length ( max ( abs(p) - b, 0.0 ) ), 0.0, 0.0 );
}
*/

vec3 trace ( vec3 from, vec3 dir, out bool hit, out int steps )
{
	vec3	p         = from;
	float	totalDist = 0.0;
	
	hit = false;
	
	for ( steps = 0; steps < 90; steps++ )
	{
		float	dist = dE ( p );
		
		if ( dist < 0.001 )
		{
			hit = true;
			break;
		}
		
		totalDist += dist;
		
		if ( totalDist > 10.0 )
			break;
			
		p += dist * dir;
	}
	
	return p;
}

float ambientOcclusion ( vec3 p, vec3 n, float eps )
{
	float ao = 1.0;
	float k  = 0.2 / eps;
	float d  = 2.0 * eps;
    
	for ( int i = 0; i < 4; i++ )
	{
		ao -= (d - dE ( p + n * d ) ) * k;
		d  += eps;
		k  *= 0.5;
	}
    
	return clamp ( ao, 0.0, 1.0 );
}

vec3 generateNormal ( vec3 z, float d )
{
    float e   = max(d * 0.5, EPS);
    float dx1 = dE(z + vec3(e, 0, 0));
    float dx2 = dE(z - vec3(e, 0, 0));
    float dy1 = dE(z + vec3(0, e, 0));
    float dy2 = dE(z - vec3(0, e, 0));
    float dz1 = dE(z + vec3(0, 0, e));
    float dz2 = dE(z - vec3(0, 0, e));
    
    return normalize ( vec3 ( dx1 - dx2, dy1 - dy2, dz1 - dz2 ) );
}

void main (void)
{
	vec3	org   = eye;
	vec3	dir   = normalize ( p - eye );
	vec3	light = vec3 ( 0.0, 0.0, 5.0 );
	bool	hit;
	int	steps;
	vec3	p = trace ( org, dir, hit, steps );
	
	if ( hit )
	{
		vec3  l  = normalize        ( light - p );
		vec3  n  = generateNormal   ( p, 0.001 );
		float ao = ambientOcclusion ( p, n, 0.001 );
		float nl = max ( 0.0, dot ( n, l ) );
		
		color = vec4 ( nl );
	} 
	else 
		color= vec4 ( 0.0, 0.0, 0.0, 1.0 );
}
