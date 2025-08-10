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

#define MAX_STEPS    			50
#define MAX_SUN_STEPS   		6
#define MARCH_SIZE   			0.16
#define ABSORPTION_COEFFICIENT  0.9
#define SCATTERING_ANISO   	 	0.3

const vec3  sunDirection = normalize ( vec3 ( 1.0, 0.0, 0.0 ) );
const vec3  sunColor     = vec3 ( 1.0,0.5,0.3 );
const float lighScale    = 2;

#include	"clouds-inc.glsl"

float    lightmarch ( vec3 pos, vec3 lightDir )
{
    float totalDensity = 0.0;
    float marchSize	= 0.03;

    for ( int step = 0; step < MAX_SUN_STEPS; step++ )
    {
		pos          += lightDir * marchSize * float ( step );
		totalDensity += scene ( pos, true );
    }

    float    transmittance = BeersLaw ( totalDensity, ABSORPTION_COEFFICIENT );

    return transmittance;
}

float raymarch ( vec3 rayOrigin, vec3 rayDirection, float offset )
{
    float    depth          	= offset * MARCH_SIZE;
    float    maxDepth       	= MAX_STEPS * MARCH_SIZE;
    float    totalTransmittance = 1.0;
    float    lightEnergy    	= 0.0;
    float    phase          	= HenyeyGreenstein ( SCATTERING_ANISO, dot ( rayDirection, sunDirection ) );

    while ( depth < maxDepth )
    {
   	 vec3    p   	= rayOrigin + depth * rayDirection;
   	 float    density = scene ( p, false );

   	 if ( density < -MARCH_SIZE )
   	 {
   		 depth += -density*0.5;
   		 continue;
   	 }

   		 // We only draw the density if it's greater than 0
   	 if ( density > 0.0 )
   	 {
   		 float lightTransmittance = lightmarch ( p, sunDirection );    //????
   		 float luminance      	  = 3*0.05 + density * phase * lighScale;

   		 totalTransmittance *= lightTransmittance;
   		 lightEnergy        += totalTransmittance * luminance;
   	 }

   	 depth += MARCH_SIZE;
    }

    return 14*0.05 * lightEnergy;
}

void main()
{
    float   blueNoise = texture   ( blueNoiseMap, uv ).r;
    vec3    org       = vec3      ( 0.0, 0.0, 5.0 );   		  // Ray Origin - camera
    vec3    dir       = normalize ( vec3 ( p.xy, -1.0 ) );    // Ray Direction
    float   res       = raymarch  ( org, dir, fract ( blueNoise + time ) * 0.5 );

	color = vec4 ( res * sunColor, 1 );
}
