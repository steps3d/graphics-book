-- vertex

#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;

out vec3 p;
out vec2 uv;

void main(void)
{
    uv      	= 0.5 * ( pos.xy + vec2 ( 1.0 ) );
    p       	= vec3 ( 1.0*pos.xy, 0.0 );
    gl_Position = vec4 ( pos.xy, 0.0, 1.0 );
}

-- fragment

#version 330 core

uniform vec3       eye;
uniform mat3       mv;
uniform float      time;
uniform sampler3D noiseMap;
uniform sampler2D blueNoiseMap;

in  vec2 uv;
in  vec3 p;
out vec4 color;

//uniform sampler2D uNoise;

#define MAX_STEPS     100
#define MARCH_SIZE    0.16

#include	"clouds-inc.glsl"

vec4 raymarch ( vec3 rayOrigin, vec3 rayDirection, float offset )
{
    float depth    = offset * MARCH_SIZE;
    vec4  res      = vec4 ( 0.0 );
    float maxDepth = MAX_STEPS * MARCH_SIZE;

    while ( depth < maxDepth )
    {
		vec3  p       = rayOrigin + depth * rayDirection;
		float density = scene ( p, false );

		if ( density < -MARCH_SIZE )
		{
			depth += -density*0.5;
			continue;
		}

			// We only draw the density if it's greater than 0
		if ( density > 0.0 )
		{
			vec4 color = vec4 ( mix ( vec3(1.0), vec3(0.0), density ), density );

			color.rgb *= color.a;
			res       += color * (1.0 - res.a);
		}

		depth += MARCH_SIZE;
    }

    return res;
}

void main()
{
    float blueNoise = texture   ( blueNoiseMap, uv ).r;
    vec3  org       = vec3      ( 0.0, 0.0, 5.0 );          // Ray Origin - camera
    vec3  dir       = normalize ( vec3 ( p.xy, -1.0 ) );    // Ray Direction
    vec4  res       = raymarch  ( org, dir, fract ( blueNoise + time ) * 0.5 );

    color = vec4 ( res.rgb, 1 );
}
