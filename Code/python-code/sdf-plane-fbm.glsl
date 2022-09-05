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

float smin( float a, float b, float k )
{
    float h = max(k-abs(a-b),0.0);
    return min(a, b) - h*h*0.25/k;
}

float smax( float a, float b, float k )
{
    float h = max(k-abs(a-b),0.0);
    return max(a, b) + h*h*0.25/k;
}

float sph( vec3 i, vec3 f, vec3 c )
{
        // random radius at grid vertex i+c (please replace this hash by
        // something better if you plan to use this for a real application)
    vec3  p = 17.0*fract( (i+c)*0.3183099+vec3(0.11,0.17,0.13) );
    float w = fract( p.x*p.y*p.z*(p.x+p.y+p.z) );
    float r = 0.7*w*w;

        // distance to sphere at grid vertex i+c
    return length(f-c) - r;
}

float sdBase( vec3 p )
{
   ivec3 i = ivec3(floor(p));
   vec3  f =       fract(p);

       // distance to the 8 corners spheres
   return min(min(min(sph(i,f,ivec3(0,0,0)),
                      sph(i,f,ivec3(0,0,1))),
                  min(sph(i,f,ivec3(0,1,0)),
                      sph(i,f,ivec3(0,1,1)))),
              min(min(sph(i,f,ivec3(1,0,0)),
                      sph(i,f,ivec3(1,0,1))),
                  min(sph(i,f,ivec3(1,1,0)),
                      sph(i,f,ivec3(1,1,1)))));
}

float plane ( vec3 p, vec4 n )
{
    return dot ( p, n.xyz ) + n.w;
}

float dE ( vec3 pos )
{
   vec3  p = mv * pos;
   float s = 1.0;
   float d = plane ( p, vec4 ( 0, 1, 0, 1 ) );

   for ( int i = 0; i < 8; i++ )
   {
               // evaluate new octave
       float n = s*sdBase(p);
    
               // add
       n = smax(n, d-0.1*s,0.3*s);
       d = smin(n, d      ,0.3*s);
    
               // prepare next octave
       p = mat3( 0.00, 1.60, 1.20,
                -1.60, 0.72,-0.96,
                -1.20,-0.96, 1.28 )*p;
       s = 0.5*s;
   }

   return d;
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
