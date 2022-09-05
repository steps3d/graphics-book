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

in  vec3 p;
out vec4 color;

	// 2D cross
float cross2D ( in vec2 p, in vec2 b, float r ) 
{
    p = abs(p); 
    p = (p.y>p.x) ? p.yx : p.xy;
    
    vec2  q = p - b;
    float k = max(q.y,q.x);
    vec2  w = (k>0.0) ? q : vec2(b.y-p.x,-k);
    
    return sign(k)*length(max(w,0.0)) + r;
}

float dE ( in vec3 p )
{
    float h = 0.2;
    vec3  pt = mv * p;

    float d = cross2D ( pt.xy,  vec2(0.8,0.35), 0.08 );
    vec2  w = vec2  ( d, abs ( pt.z ) - h );

    return min ( max ( w.x, w.y ),0.0) + length ( max ( w, 0.0 ) );
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
        vec3  l  = normalize      ( light - p );
        vec3  n  = generateNormal ( p, 0.001 );
        float nl = max            ( 0.0, dot ( n, l ) );
        
        color = vec4 ( nl );
    }
    else
        color = vec4 ( 0.0, 0.0, 0.0, 1.0 );
}
