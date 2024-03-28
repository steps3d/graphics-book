-- compute
#version 430

layout( local_size_x = 64 ) in;

uniform float deltaT;
uniform float maxDist     	= 45.0;

uniform sampler2D   noiseMap;

layout(std430, binding = 0) buffer Pos
{
    vec4 position [];
};

const float eps = 1.0 / 256.0;
const vec2 dx = vec2 ( eps, 0 );
const vec2 dy = vec2 ( 0, eps );

void main()
{
    uint  idx  = gl_GlobalInvocationID.x;
    vec2  p	= position [idx].xy;
    float a	= textureLod ( noiseMap, 0.25*p + dy, 0 ).r - textureLod ( noiseMap, 0.25*p - dy, 0 ).r;
    float b	= textureLod ( noiseMap, 0.25*p + dx, 0 ).r - textureLod ( noiseMap, 0.25*p - dx, 0 ).r;
    vec2  curl = vec2 ( a, -b ) / eps;
   	 
    position [idx] = vec4 ( p + curl*0.00001, 0.0, 1.0 );
}


