-- vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform vec3 lightDir;

out vec3 l;
out vec3 n;
out vec2 tx;

void main(void)
{
	vec4 pt = mv * vec4 ( pos, 1.0 );

        tx = tex;
	n  = normalize ( nm * normal );
	l  = normalize ( lightDir );
	//v  = normalize ( eye   - p.xyz );

	gl_Position  = proj * pt;
}

-- fragment
#version 330 core

in  vec2 tx;
in  vec3 l;
in  vec3 n;
out vec4 color;		// output value

uniform sampler2D image;

const vec3  lightColor = vec3 ( 1.0 );
//const float gamma = 2.2;
//const float pi    = 3.1415926;

vec4 hash4 ( in vec2 p ) 
{ 
    return fract ( sin ( vec4 ( 1.0 + dot ( p, vec2(37.0,17.0)), 
                                2.0 + dot ( p, vec2(11.0,47.0)),
                                3.0 + dot ( p, vec2(41.0,29.0)),
                                4.0 + dot ( p, vec2(23.0,31.0))))*103.0); 
}

vec4 textureNoTile ( in vec2 uv )
{
     ivec2 iuv = ivec2 ( floor ( uv ) );
     vec2  fuv = fract ( uv );

	// generate per-tile transform
    vec4 ofa = hash4 ( iuv + ivec2(0,0) );
    vec4 ofb = hash4 ( iuv + ivec2(1,0) );
    vec4 ofc = hash4 ( iuv + ivec2(0,1) );
    vec4 ofd = hash4 ( iuv + ivec2(1,1) );
  
    vec2 ddx = dFdx ( uv );
    vec2 ddy = dFdy ( uv );

	// transform per-tile uvs
    ofa.zw = sign ( ofa.zw - 0.5 );
    ofb.zw = sign ( ofb.zw - 0.5 );
    ofc.zw = sign ( ofc.zw - 0.5 );
    ofd.zw = sign ( ofd.zw - 0.5 );
    
	// uv's, and derivatives (for correct mipmapping)
    vec2 uva  = uv  * ofa.zw + ofa.xy;
    vec2 ddxa = ddx * ofa.zw;
    vec2 ddya = ddy * ofa.zw;
    vec2 uvb  = uv  * ofb.zw + ofb.xy;
    vec2 ddxb = ddx * ofb.zw;
    vec2 ddyb = ddy * ofb.zw;
    vec2 uvc  = uv  * ofc.zw + ofc.xy;
    vec2 ddxc = ddx * ofc.zw;
    vec2 ddyc = ddy * ofc.zw;
    vec2 uvd  = uv  * ofd.zw + ofd.xy;
    vec2 ddxd = ddx * ofd.zw;
    vec2 ddyd = ddy * ofd.zw;
        
	// fetch and blend
    vec2 b = smoothstep ( 0.25,0.75, fuv );
    
    return mix( mix( textureGrad ( image, uva, ddxa, ddya ), 
                     textureGrad ( image, uvb, ddxb, ddyb ), b.x ), 
                mix( textureGrad ( image, uvc, ddxc, ddyc ),
                     textureGrad ( image, uvd, ddxd, ddyd ), b.x), b.y );
}

void main ()
{
    color = textureNoTile ( tx );
//color = texture ( image, tx );
}
