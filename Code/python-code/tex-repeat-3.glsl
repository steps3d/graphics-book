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

float sum ( vec3 v ) 
{ 
   return v.x + v.y + v.z; 
}

vec2 hash ( in float x )
{
    return sin ( x * vec2 ( 3.0, 7.0 ) );
}

vec4 textureNoTile ( in vec2 x, in float v )
{
	// sample variation pattern    
    float k = texture ( image, 0.005*x ).x; // cheap (cache friendly) lookup    
    
	// compute index    
    float index = k * 8.0;
    float f     = fract ( index );

	// offsets for the different virtual patterns    
    float ia    = floor ( index    );
    vec2  offsa = hash  ( ia       );
    vec2  offsb = hash  ( ia + 1.0 );

	// compute derivatives for mip-mapping    
    vec2 dx = dFdx ( x );
    vec2 dy = dFdy ( x );

	// sample the two closest virtual patterns    
    vec4 cola = textureGrad ( image, x + offsa, dx, dy );
    vec4 colb = textureGrad ( image, x + offsb, dx, dy );

	// interpolate between the two virtual patterns    
    return mix ( cola, colb, smoothstep ( 0.2, 0.8, f - 0.1*sum(cola.xyz-colb.xyz)));
}

void main ()
{
    color = textureNoTile ( tx, 1.0 );
//color = texture ( image, tx );
}
