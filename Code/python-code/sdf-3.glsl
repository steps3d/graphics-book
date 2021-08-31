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

in  vec3 p;
out vec4 color;

float length8 ( vec2 p )
{
	p = abs ( p );
	
	return pow ( pow ( p.x, 8.0 ) + pow ( p.y, 8.0 ), 1.0 / 8.0 );
}

float length8 ( vec3 p )
{
	p = abs ( p );
	
	return pow ( pow ( p.x, 8.0 ) + pow ( p.y, 8.0 ) + pow ( p.z, 8.0 ), 1.0 / 8.0 );
}

float dSphere ( vec3 p )
{
	return length ( p ) - 1.0;
}

float dBox ( vec3 p, vec3 b )
{
	return length ( max ( abs ( p ) - b, 0.0 ) );
}

float dTorus ( vec3 p, vec2 t )
{
	vec2	q = vec2 ( length ( p.xz ) - t.x, p.y );
	
	return length ( q ) - t.y;
}

float dCyl ( vec3 p, vec3 c )
{
	return length ( p.xz - c.xy ) - c.z;
}

float dPlane ( vec3 p, vec4 n )
{
	return dot ( p, n.xyz ) + n.w;
}

float dCone ( vec3 p, vec2 c )
{
	float q = length ( p.yz );
	
	return dot ( c, vec2 ( q, p.x ) );
}

float dCappedCyl ( vec3 p, vec2 h )
{
	vec2 d = abs ( vec2 ( length ( p.xz ), p.y ) ) - h;
	
	return min ( max ( d.x, d.y ), 0.0 ) + length ( max ( d, 0.0 ) );
}

float smin ( float a, float b, float k )
{
	float res = exp ( -k*a ) + exp ( -k*b );

	return -log ( res ) / k;
}

float dTorus82 ( vec3 p, vec2 t )
{
	vec2	q = vec2 ( length8 ( p.xz ) - t.x, p.y );
	
	return length ( q ) - t.y;
}

float dTorus28 ( vec3 p, vec2 t )
{
	vec2	q = vec2 ( length ( p.xz ) - t.x, p.y );
	
	return length8 ( q ) - t.y;
}

float dTorus88 ( vec3 p, vec2 t )
{
	vec2	q = vec2 ( length8 ( p.xz ) - t.x, p.y );
	
	return length8 ( q ) - t.y;
}


float dE ( vec3 p )
{
	p *= mv;
	
	//return dCyl ( p, vec3 ( 0.0, 0.0, 0.3 ) );
	//return dTorus ( p, vec2 ( 0.43, 0.2 ) );
	//return dBox ( p, vec3 ( 0.3, 0.1, 0.2 ) );
	//return dCone ( p, vec2 ( 0.3, 0.45 ) );
	//return dCappedCyl ( p, vec2 ( 0.3, 0.1 ) );

	//return dTorus82 ( p, vec2 ( 0.43, 0.2 ) );
	//return dTorus28 ( p, vec2 ( 0.43, 0.2 ) );
	//return dTorus88 ( p, vec2 ( 0.43, 0.2 ) );

//	return max ( dTorus ( p, vec2 ( 0.43, 0.2    ) ), 
//                 dBox   ( p, vec3 ( 0.4, 0.5, 0.2 ) ) );
	
	return smin ( dTorus ( p, vec2 ( 0.43, 0.2    ) ), 
                     dBox   ( p, vec3 ( 0.3, 0.4, 0.2 ) ), 7.0 );
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
	bool	hit;
	int		steps;
	vec3	org   = eye;
	vec3	dir   = normalize ( p - eye );
	vec3	light = vec3  ( 0.0, 0.0, 5.0 );
	vec3	p     = trace ( org, dir, hit, steps );
	
	if ( hit )
	{
		vec3  l  = normalize        ( light - p );
		vec3  n  = generateNormal   ( p, 0.001 );
		float nl = max ( 0.0, dot ( n, l ) );
		
		color = vec4 ( nl );
	} 
	else 
		color= vec4 ( 0.0, 0.0, 0.0, 1.0 );
}
