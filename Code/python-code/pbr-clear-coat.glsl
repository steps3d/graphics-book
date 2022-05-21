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
    
                // convert to TBN
    v  = vec3 ( dot ( v, t ), dot ( v, b ), dot ( v, n ) );
    l  = vec3 ( dot ( l, t ), dot ( l, b ), dot ( l, n ) );
    h  = vec3 ( dot ( h, t ), dot ( h, b ), dot ( h, n ) );
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

uniform float strength;

float sqr ( in float x )
{
    return x * x;
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
/*
vec3 fr ( in float nl, in float nv, in float nh, in float vh, in vec3 f0, in float roughness )
{
    if ( nl < 0.0 )
        return vec3 ( 0.0 );
    if ( nv < 0.0 )
        return vec3 ( 0.0 );

//    float D = D_blinn(roughness, nh );
//    float D = D_beckmann(roughness, nh );
    float D = D_GGX     ( roughness, nh );

//    float G = G_schlick ( roughness, nv, nl );    // XXX
    float G = G_neumann ( nl, nv );
//    float G = G_klemen ( nl, nv, vh );
//    float G = G_default ( nl, nh, nv, vh );

    return f0 * D * G / (nl * nv);
}
*/
vec3 fc ( in vec3 c, in float strength, in float roughnessMat, in float roughnessCC, in float nl, in float nv, in float nh, in float lh, in float vh )
{
    vec3    F0Mat = mix ( vec3(FDiel), c, 1 );
    vec3    F0CC  = vec3(FDiel) * strength;
    float    D     = D_GGX     ( roughnessMat, nh );
    float    Dc    = D_GGX     ( roughnessCC,  nh );
    float    G     = G_neumann ( nl, nv );
    vec3     FMat  = fresnel   ( F0Mat, vh );
    vec3     Fcc   = fresnel   ( F0CC,  vh );
    float    V     = 1.0 / sqr ( lh );                // use simplified G
    vec3    fd    = c / pi;
    vec3    fr    = FMat * D * G / (nl * nv);
    vec3    fc    = V * Dc * Fcc;
    //vec3    c2    = vec3 ( 0, 0, 1 );
    
    //return c / pi * ( 1 - Fcc ) + fr * ( 1 - Fcc ) + fc;
    //return c * ((fd + fr * (vec3(1.0) - Fcc))*(vec3(1.0) - Fcc) + fc);
    
    return fd * ( vec3 ( 1.0 ) - Fcc ) * fr * ( vec3 ( 1.0 ) - Fcc ) + fc;
}

void main ()
{
    vec3    n          = vec3 ( 0, 0, 1 );        // unperturbed normal
//    vec3    base       = vec3 ( 1, 1, 0 );        // yellow
    vec3    base       = vec3 ( 1, 0.5, 0.24 );
//    float    roughness  = 0.25;
//    float    metallness = 0.0;

    base = pow ( base, vec3 ( gamma ) );
    
    vec3  n2   = normalize ( n );
    vec3  l2   = normalize ( l );
    vec3  h2   = normalize ( h );
    vec3  v2   = normalize ( v );
    float nv   = max ( 0.0, dot ( n2, v2 ));
    float nl   = max ( 0.0, dot ( n2, l2 ));
    float nh   = max ( 0.0, dot ( n2, h2 ));
    float hl   = max ( 0.0, dot ( h2, l2 ));
    float vh   = max ( 0.0, dot ( h2, v2 ));
    vec3  c    = fc ( base, strength, 0.125, 0.7, nl, nv, nh, hl, vh) * nl;

    color = pow ( vec4 ( c, 1.0 ), vec4 ( 1.0 / gamma ) );
    //color = vec4(c,1);
}
