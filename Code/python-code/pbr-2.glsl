-- vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
//layout(location = 4) in vec3 binormal;

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform	vec3 eye;		// eye position
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
	vec4 p        = mv * vec4 ( pos, 1.0 );
	vec3 binormal = cross ( normal, tangent );

//p.xyz *= 50.0;

	gl_Position  = proj * p;

	tx = tex;		// for knot
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
out vec4 color;		// output value

uniform sampler2D albedoMap;
uniform sampler2D mrMap;
uniform sampler2D normalMap;
uniform vec3 lightDir;

const vec3  lightColor = vec3 ( 1.0 );
const float gamma = 2.2;
const float pi    = 3.1415926;
const float FDiel = 0.04;		// Fresnel for dielectrics

vec3 fresnel ( in vec3 f0, in float product )
{
	product = clamp ( product, 0.0, 1.0 );		// saturate
	
	return mix ( f0, vec3 (1.0), pow(1.0 - product, 5.0) );
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

vec3 cookTorrance ( in float nl, in float nv, in float nh, in float vh, in vec3 f0, in float roughness )
{
//	float D = D_blinn(roughness, nh );
//      float D = D_beckmann(roughness, nh );
	float D = D_GGX     ( roughness, nh );

//	float G = G_schlick ( roughness, nv, nl );	// XXX
	float G = G_neumann ( nl, nv );
//	float G = G_klemen ( nl, nv, vh );
//	float G = G_default ( nl, nh, nv, vh );

	return f0 * D * G;
}

void main ()
{
	vec3 base        = texture ( albedoMap,     tx).xyz;
	vec3 n           = texture ( normalMap,     tx).xyz * 2.0 - vec3 ( 1.0 );
	vec2 mr          = texture ( mrMap,         tx).xy;
	float roughness  = mr.x;
	float metallness = mr.y;

//const float roughness = 0.09;
//const vec4  r0   = vec4 ( 1.0, 0.92, 0.23, 1.0);

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
	
	vec3 F0          = mix ( vec3(FDiel), base, metallness );
	vec3 specFresnel = fresnel ( F0, nv );	// hv
	vec3 spec        = cookTorrance ( nl, nv, nh, vh, specFresnel, roughness ) * nl / max ( 0.001, 4.0 * nl * nv );
	vec3 diff        = (vec3(1.0) - specFresnel) * nl / pi;

	color = pow ( vec4 ( ( diff * mix ( base, vec3(0.0), metallness) + spec ) * lightColor, 1.0 ), vec4 ( 1.0 / gamma ) );

/*
	float r2   = roughness * roughness;
	float nh2  = nh * nh;
	float ex   = -(1.0 - nh2)/(nh2 * r2);
	float d    = pow(2.7182818284, ex ) / (r2*nh2*nh2); 
	
	vec4  f    = mix(vec4(pow(1.0 - nv, 5.0)), vec4(1.0), r0);
	
					// default G
	float x    = 2.0 * nh / dot(v2, h);
	float g    = min(1.0, min (x * nl, x * nv));
	
					// resulting color
	vec4  ct   = f*(0.25 * d * g / nv);
	float diff = max(nl, 0.0);
	float ks   = 0.5;

	color = diff * vec4(base,1.0) + ks * ct;
*/
}
