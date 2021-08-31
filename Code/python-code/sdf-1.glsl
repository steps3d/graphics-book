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

uniform vec3 	  eye;

in  vec3 p;
out vec4 color;

		// Unit sphere
float dE ( vec3 p )
{
	return length ( p ) - 1.0;
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

void main (void)
{
	vec3	org   = eye;
	vec3	dir   = normalize ( p - eye );
	vec3	light = vec3 ( 0.0, 0.0, 5.0 );
	bool	hit;
	int	steps;
	vec3	p = trace ( org, dir, hit, steps );
	
	if ( hit )
		color = vec4 ( 1.0 );
	else 
		color = vec4 ( 0.0, 0.0, 0.0, 1.0 );
}
