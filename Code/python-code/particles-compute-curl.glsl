-- compute
#version 430

layout( local_size_x = 64 ) in;

uniform float deltaT;
uniform float maxDist = 45.0;

uniform sampler3D   noiseMap;

layout(std430, binding = 0) buffer Pos
{
    vec4 position [];
};

const float eps = 1.0 / 64.0;
const vec3 dx   = vec3 ( eps, 0, 0 );
const vec3 dy   = vec3 ( 0, eps, 0 );
const vec3 dz   = vec3 ( 0, 0, eps );
const float scale = 0.2;

void main()
{
    uint  idx  = gl_GlobalInvocationID.x;
    vec3  p	= position [idx].xyz;
    float nx   = textureLod ( noiseMap, scale * p + dx, 0 ).r - textureLod ( noiseMap, scale * p - dx, 0 ).r;
    float ny   = textureLod ( noiseMap, scale * p + dy, 0 ).r - textureLod ( noiseMap, scale * p - dy, 0 ).r;
    float nz   = textureLod ( noiseMap, scale * p + dz, 0 ).r - textureLod ( noiseMap, scale * p - dz, 0 ).r;
    vec3  curl = vec3 ( ny - nz, nz - nx,  nx - ny ) / eps;
   	 
    position [idx] = vec4 ( p + curl*0.0003, 1.0 );
}


