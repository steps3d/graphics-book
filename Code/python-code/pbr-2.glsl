-- vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 binormal;

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform    vec3 eye;        // eye position
uniform vec3 lightDir;

out vec3 l;
out vec3 h;
out vec3 v;
out vec3 t;
out vec3 b;
out vec3 n;
out vec2 tx;

void main(void)
{
    vec4 p = mv * vec4 ( pos, 1.0 );

    gl_Position  = proj * p;

    tx = tex * vec2 ( 1, 6 );        // for knot
    n  = normalize (nm * normal);
    t  = normalize (nm * tangent);
    b  = normalize (nm * binormal);
    l  = normalize (lightDir);
    v  = normalize (eye   - p.xyz);
    h  = normalize (l + v);
/*    
                // convert to TBN
    v  = vec3 ( dot ( v, t ), dot ( v, b ), dot ( v, n ) );
    l  = vec3 ( dot ( l, t ), dot ( l, b ), dot ( l, n ) );
    h  = vec3 ( dot ( h, t ), dot ( h, b ), dot ( h, n ) );
*/
}

-- fragment
#version 330 core

in  vec2 tx;
in  vec3 l;
in  vec3 h;
in  vec3 v;
in  vec3 t;
in  vec3 b;
in  vec3 n;
out vec4 color;        // output value

const vec3  lightColor = vec3 ( 1.0 );
const float gamma = 2.2;
const float pi    = 3.1415926;
const float FDiel = 0.04;        // Fresnel for dielectrics

uniform float roughness;
uniform float metallness;

float sqr ( in float x )
{
    return x*x;
}

    // optimized x^5 without special functions
float pow5 ( in float x )
{
    float x2 = x * x;
    
    return x2*x2*x;
}
    
vec3 fresnel ( in vec3 f0, in float product )
{
    product = clamp ( product, 0.0, 1.0 );        // saturate
    
    return mix ( f0, vec3 (1.0), pow5(1.0 - product) );
}

float fresnel ( in float f0, in float f90, in float vh )
{
    return f0 + (f90 - f0) * pow5(1.0 - vh);
}

float D_blinn(in float roughness, in float NdH)
{
    float m = roughness * roughness;
    float m2 = m * m;
    float n = 2.0 / m2 - 2.0;
    return (n + 2.0) / (2.0 * pi) * pow(NdH, n);
}

float D_beckmann ( in float roughness, in float NdH )
{
    float m    = roughness * roughness;
    float m2   = m * m;
    float NdH2 = NdH * NdH;
    
    return exp( (NdH2 - 1.0) / (m2 * NdH2) ) / (pi * m2 * NdH2 * NdH2);
}

float D_GGX ( in float roughness, in float NdH )
{
    float m  = roughness * roughness;
    float m2 = m * m;
    float NdH2 = NdH * NdH;
    float d  = (m2 - 1.0) * NdH2 + 1.0;
    
    return m2 / (pi * d * d);
}

    // here we may use (roughness, anisotropy)
    // sigma = vec2 ( roughness * (1 + anisotropy), roughness * (1 - anisotropy) )
float D_GGX_Aniso ( in vec2 sigma, in float nh, in float th, in float bh )
{
    return 1.0 / (pi * sigma.x * sigma.y * sqr ( sqr(nh) + sqr (th/sigma.x) + sqr (bh/sigma.y) ) );
}

float G_schlick ( in float roughness, in float nv, in float nl )
{
    float k = roughness * roughness * 0.5;
    float V = nv * (1.0 - k) + k;
    float L = nl * (1.0 - k) + k;
    
    return 0.25 / (V * L);
}

float G_neumann ( in float nl, in float nv )
{
    return nl * nv / max ( nl, nv );
}

float G_klemen ( in float nl, in float nv, in float vh )
{
    return nl * nv / (vh * vh );
}

float G_default ( in float nl, in float nh, in float nv, in float vh )
{
    return min ( 1.0, min ( 2.0*nh*nv/vh, 2.0*nh*nl/vh ) );
}

vec3 cookTorrance ( in float nl, in float nv, in float nh, in float vh, in vec3 f0, in float roughness, in float th, in float bh )
{
    if ( nl < 0.0 )
        return vec3 ( 0.0 );
    if ( nv < 0.0 )
        return vec3 ( 0.0 );

//    float D = D_blinn(roughness, nh );
//    float D = D_beckmann(roughness, nh );
//    float D = D_GGX     ( roughness, nh );

    float D = D_GGX_Aniso ( vec2 ( 0.25, 0.025 ), nh, th, bh );

//    float G = G_schlick ( roughness, nv, nl );    // XXX
    float G = G_neumann ( nl, nv );
//    float G = G_klemen ( nl, nv, vh );
//    float G = G_default ( nl, nh, nv, vh );

    return f0 * D * G;
}

float    burley ( in float r, in float nv, in float nl, in float lh )
{
    float f90           = 0.5 + 2.0 * r * lh * lh;
    float lightScatter = fresnel ( 1.0, f90, nl );
    float viewScatter  = fresnel ( 1.0, f90, nv );
    
    return lightScatter * viewScatter / pi;
}

void main ()
{
//    vec3    n          = vec3 ( 0, 0, 1 );        // unperturbed normal
//    vec3    base       = vec3 ( 1, 1, 0 );        // yellow
    vec3    base       = vec3 ( 1, 0.5, 0.24 );
//    float    roughness  = 0.25;
    float    metallness = 1.0;

    base = pow ( base, vec3 ( gamma ) );
    
    vec3  n2   = normalize ( n );
    vec3  l2   = normalize ( l );
    vec3  h2   = normalize ( h );
    vec3  v2   = normalize ( v );
    vec3  t2   = normalize ( t );
    vec3  b2   = normalize ( b );
    float nv   = max ( 0.0, dot ( n2, v2 ));
    float nl   = max ( 0.0, dot ( n2, l2 ));
    float hl   = max ( 0.0, dot ( h2, l2 ));
    float vh   = max ( 0.0, dot ( h2, v2 ));
    float nh   = dot ( n2, h2 );
    float th   = dot ( h2, t2 );
    float bh   = dot ( h2, b2 );
    
    vec3 F0          = mix ( vec3(FDiel), base, metallness );
    vec3 specFresnel = fresnel ( F0, vh );    //nv );    // hv
    vec3 spec        = cookTorrance ( nl, nv, nh, vh, specFresnel, roughness, th, bh ) / max ( 0.001, 4.0 * nv );
    vec3 diff        = (vec3(1.0) - specFresnel) * nl / pi;

    color = pow ( vec4 ( ( diff * mix ( base, vec3(0.0), metallness) + spec ), 1.0 ), vec4 ( 1.0 / gamma ) );
}
