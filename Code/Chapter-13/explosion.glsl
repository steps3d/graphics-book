-- vertex

#version 430 core

uniform mat4 proj;
uniform mat4 mv;
uniform float time;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

uniform sampler3D noiseMap;
uniform vec3	  factor;
uniform float	  period;
uniform float	  dispScale;

out vec3 pp;

vec3	turbulence ( vec3 coord )
{
	return 0.5 * texture ( noiseMap, coord ).xyz + 0.25 * texture ( noiseMap, 2.0 * coord ).xyz + 0.125 * texture ( noiseMap, 4.0 * coord ).xyz;
}

float	animate ()
{
	return fract ( time / period );
}

void main(void)
{
	pp  = pos * 0.5;
	
	float	d    = dot ( turbulence ( pp ).rgb, factor ) * dispScale;
		
	gl_Position = proj * mv * vec4 ( (pos + normal*d) * animate (), 1.0 );
}

-- fragment

#version 430 core

uniform sampler3D noiseMap;
uniform sampler2D colorMap;
uniform	float     time;
uniform vec3	  factor;
uniform float	  period;
uniform float	  bias;

in vec3 pp;

out vec4 color;

vec3	turbulence ( vec3 coord )
{
	return 0.5 * texture ( noiseMap, coord ).xyz + 0.25 * texture ( noiseMap, 2.0 * coord ).xyz + 0.125 * texture ( noiseMap, 4.0 * coord ).xyz;// + 0.0625 * texture ( noiseMap, 8.0 * coord ).xyz;
}

float	animate ()
{
	return fract ( time / period );
}

void main (void)
{
	vec2	range = vec2 ( 0.0, 1.0 );
	float	d     = dot  ( turbulence ( pp ), factor ) * (range.y - range.x) + range.x + animate () - bias;

	if ( d > 0.5 )
		discard;
		
	color = texture ( colorMap, vec2 ( d, 0.5 ) );
}
