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

#define MAX_STEPS    			 50
#define MARCH_SIZE   			 0.16
#define ABSORPTION_COEFFICIENT   0.9

#include	"clouds-inc.glsl"

const vec3 sunDirection = normalize ( vec3 ( 1.0, 0.0, 0.0 ) );
const vec3 sunColor 	= vec3 ( 1.0,0.5,0.3 );

float raymarch ( vec3 rayOrigin, vec3 rayDirection, float offset )
{
    float    depth              = offset * MARCH_SIZE;
    float    maxDepth           = MAX_STEPS * MARCH_SIZE;
    float    totalTransmittance = 1.0;
    float    lightEnergy    	= 0.0;

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
   		 float transmittance = BeersLaw ( density * MARCH_SIZE, ABSORPTION_COEFFICIENT );
   		 float luminance     = density;

   		 totalTransmittance *= transmittance;
   		 lightEnergy        += totalTransmittance * luminance;
   	 }

   	 depth += MARCH_SIZE;
    }

    return 0.05*lightEnergy;
}

void main()
{
    float blueNoise = texture   ( blueNoiseMap, uv ).r;
    vec3  org       = vec3      ( 0.0, 0.0, 5.0 );   		// Ray Origin - camera
    vec3  dir       = normalize ( vec3 ( p.xy, -1.0 ) );    // Ray Direction
    float res       = raymarch  ( org, dir, fract ( blueNoise + time ) * 0.5 );

   	 // Sun and Sky
    vec3  sunColor     = vec3      ( 1.0,0.5,0.3 );
    float sun          = clamp     ( dot ( sunDirection, dir), 0.0, 1.0) ;

   	 // Base sky color
    color.rgb = vec3(0.7,0.7,0.90);

   	 // Add vertical gradient
    color.rgb -= 0.8 * vec3(0.90,0.75,0.90) * dir.y;

   	 // Add sun color to sky
    color.rgb += 0.5 * sunColor * pow ( sun, 10.0 );

   	 // Add cloud color
    color = vec4 ( color.rgb + res * sunColor, 1 );

}
