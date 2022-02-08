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
    vec2 p = floor ( uv );
    vec2 f = fract ( uv );
	
	// derivatives (for correct mipmapping)
    vec2 ddx = dFdx ( uv );
    vec2 ddy = dFdy ( uv );
    
	// voronoi contribution
    vec4  va = vec4 ( 0.0 );
    float wt = 0.0;

    for ( int j = -1; j <= 1; j++ )
        for ( int i = -1; i <= 1; i++ )
        {
            vec2  g = vec2  ( float(i), float(j) );
            vec4  o = hash4 ( p + g );
            vec2  r = g - f + o.xy;
            float d = dot ( r, r );
            float w = exp ( -5.0*d );
            vec4  c = textureGrad ( image, uv + o.zw, ddx, ddy );

            va += w*c;
            wt += w;
        }
	
	// normalization
    return va/wt;
}

void main ()
{
    color = textureNoTile ( tx );
//color = texture ( image, tx );
}
