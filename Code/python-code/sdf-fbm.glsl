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

#define    EPS                1.0e-5

uniform vec3       eye;
uniform mat3       mv;

in  vec3 p;
out vec4 color;

#define NOISE 1

float box ( in vec3 pos, in vec3 size )
{
    vec3 q = abs ( pos ) - size;

    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float hash ( in vec3 p )
{
    p  = 17.0*fract( p*0.3183099+vec3(.11,.17,.13) );
    
    return fract ( p.x*p.y*p.z*(p.x+p.y+p.z) );
}

float smax ( in float a, in float b, in float k )
{
    float h = max ( k - abs ( a - b ), 0.0 );
    
    return max ( a, b ) + h * h * 0.25 / k;
}

float base( in vec3 p )
{
#if NOISE==0
    vec3 i = floor(p);
    vec3 f = fract(p);

    #define RAD(r) ((r)*(r)*0.7)
    #define SPH(i,f,c) length(f-c)-RAD(hash(i+c))
    
    return min(min(min(SPH(i,f,vec3(0,0,0)),
                       SPH(i,f,vec3(0,0,1))),
                   min(SPH(i,f,vec3(0,1,0)),
                       SPH(i,f,vec3(0,1,1)))),
               min(min(SPH(i,f,vec3(1,0,0)),
                       SPH(i,f,vec3(1,0,1))),
                   min(SPH(i,f,vec3(1,1,0)),
                       SPH(i,f,vec3(1,1,1)))));
#else
    const float K1 = 0.333333333;
    const float K2 = 0.166666667;
    
    vec3 i = floor(p + (p.x + p.y + p.z) * K1);
    vec3 d0 = p - (i - (i.x + i.y + i.z) * K2);
    
    vec3 e = step(d0.yzx, d0);
    vec3 i1 = e*(1.0-e.zxy);
    vec3 i2 = 1.0-e.zxy*(1.0-e);
    
    vec3 d1 = d0 - (i1  - 1.0*K2);
    vec3 d2 = d0 - (i2  - 2.0*K2);
    vec3 d3 = d0 - (1.0 - 3.0*K2);
    
    float r0 = hash( i+0.0 );
    float r1 = hash( i+i1 );
    float r2 = hash( i+i2 );
    float r3 = hash( i+1.0 );

    #define SPH(d,r) length(d)-r*r*0.55

    return min( min(SPH(d0,r0),
                    SPH(d1,r1)),
                min(SPH(d2,r2),
                    SPH(d3,r3)));
#endif
}

const mat3 m = mat3( 0.00,  1.60,  1.20,
                    -1.60,  0.72, -0.96,
                    -1.20, -0.96,  1.28 );
                    
float fbm( vec3 p, float d )
{
   float s = 0.5;
   
   for( int i = 0; i < 7; i++ )
   {
       // evaluate new octave
       float n = s*base ( p );
    
        // subtract
       d = smax( d, -n, 0.2*s );
    
        // prepare next octave
       p = m * p;
       s = 0.5 * s;
   }
   return d;
}

float dE ( in vec3 pos )
{
    vec3  p  = mv * pos;
    float d  = box ( p, vec3 ( 0.5 ) );
    float dt = fbm ( p + 0.5, d );

    //dt.y = 1.0+dt.y*2.0; dt.y = dt.y*dt.y;    
    return dt;
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

float softShadow ( in vec3 pos, in vec3 l, float tmin, float tmax, float w )
{
    float t   = tmin;
    float res = 1.0;
    
    for( int i = 0; i < 128; i++ )
    {
         float h = dE ( pos + t * l );
        
        res = min   ( res, h/(w*t) );        // 1/w if OK, near to zero if shadowed
        t  += clamp ( h, 0.005, 0.50 );        // limit step
        
        if ( res < -1.0 || t > tmax )         // early exit
            break;
    }
    
    res = max ( res, -1.0 );                 // clamp to [-1,1]

    return 0.25*(1.0+res)*(1.0+res)*(2.0-res); // smoothstep
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
        
        color = vec4 ( nl ) * softShadow ( p + 0.001 * n, l, 0, 1, 1 );
    }
    else
        color = vec4 ( 0.0, 0.0, 0.0, 1.0 );
}

