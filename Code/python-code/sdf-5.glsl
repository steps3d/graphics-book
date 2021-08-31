-- vertex

#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;

out vec3 p;

void main(void)
{
	p           = vec3 ( 0.9*pos.xy, 0.0 );
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

const float scale  = 1.7;
const float offset = 0.5;

float dE ( vec3 z )
{
	z *= mv;
	
	float	r;
	int		n = 0;
	
	while ( n < 58 )
	{
		if ( z.x+z.y < 0 )
			z.xy = -z.yx;		// fold 1
			
		if ( z.x+z.z < 0 )
			z.xz = -z.zx; 		// fold 2
			
		if ( z.y+z.z < 0 )
			z.zy = -z.yz; 		// fold 3	
			
		z = z*scale - offset*(scale-1.0);
		n++;
	}
	
	return (length (z) ) * pow(scale, -float(n));
}

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
	float aoIntensity = 0.2;
	float ao = 1.0;                 // Start at full output color intensity
	float k = aoIntensity / eps;    // Set intensity factor
	float d = 2.0 * eps;            // Start ray a little off the surface
    
	 for ( int i = 0; i < 4; i++ )
	{
		ao -= (d - dE ( p + n * d )) * k;
		d += eps;
		k *= 0.5;               // AO contribution drops as we move further from the surface 
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
