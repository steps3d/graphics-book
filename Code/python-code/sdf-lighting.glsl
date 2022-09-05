-- vertex

#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;

uniform float fov;

out vec3 p;

void main(void)
{
    p           = vec3 ( fov * pos.xy, 0.0 );
    gl_Position = vec4 ( pos.xy, 0.0, 1.0 );
}

-- fragment

#version 330 core

#define    EPS                1.0e-5

uniform vec3       eye;
uniform mat3       mv;
uniform float      time;

in  vec3 p;
out vec4 color;

float sphere ( vec3 p, in vec3 c, in float r )
{
	return length ( p - c ) - r;
}

float length8 ( in vec2 p )
{
    p = abs ( p );

    return pow ( pow ( p.x, 8.0 ) + pow ( p.y, 8.0 ), 1.0/ 8.0 );
}

float length8 ( in vec3 p )
{
    p = abs ( p );

    return pow ( pow ( p.x, 8.0 ) + pow ( p.y, 8.0 ) + pow ( p.z, 8.0 ), 1.0/ 8.0 );
}

float torus ( vec3 p, vec2 t )
{
	vec2	q = vec2 ( length8 ( p.xz ) - t.x, p.y );
	
	return length8 ( q ) - t.y;
}

float smin ( float a, float b, float k )
{
	float res = exp ( -k*a ) + exp ( -k*b );

	return -log ( res ) / k;
}

float dE ( in vec3 pos )
{
    vec3  pt = mv * pos;
    float r  = 0.5  + 0.03 * sin(20.0 * pt.x + time);

    //return torus ( pt, vec2 ( 0.3, 0.13 ) );
    return smin ( torus ( pt, vec2 ( 0.3, 0.13 ) ),  sphere ( pt, vec3 ( 1, 0, 0 ), r ), 8.0 );
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

vec3 trace ( vec3 from, vec3 dir, out bool hit, out int steps )
{
    vec3    p         = from;
    float    totalDist = 0.0;
    
    hit = false;
    
    for ( steps = 0; steps < 90; steps++ )
    {
        float    dist = dE ( p );
        
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

const float pi         = 3.1415926;
const float specPower  = 70.0;
const float normFactor = (specPower + 2.0) / (4.0 * pi * (2.0 - pow ( 2.0, -specPower/2.0)));
const vec4  specColor  = vec4 ( 0, 1, 0, 1 );

void main (void)
{
    bool    hit;
    int    steps;
    vec3    org   = eye;
    vec3    dir   = normalize ( p - eye );
    vec3    light = vec3  ( 0.0, 0.0, 5.0 );
    vec3    p     = trace ( org, dir, hit, steps );
    
    if ( hit )
    {
	vec3  l  = normalize        ( light - p );
        vec3  v  = normalize        ( eye - p );
	vec3  n  = generateNormal   ( p, 0.001 );
	float nl = max ( 0.0, dot ( n, l ) );
        vec3  h  = normalize ( l + v );
        float hn = max ( 0.0, dot ( h, n ) );
        float sp = pow ( hn, specPower ) * normFactor;
		
	color = 0.5*vec4 ( nl ) + 0.5 * sp * specColor;
    }
    else
        color = vec4 ( 0.0, 0.0, 0.0, 1.0 );
}
