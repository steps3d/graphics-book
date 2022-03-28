-- vertex

#version 330 core

layout(location = 0) in vec4 pos;

out vec2 tex;

void main(void)
{
    tex         = vec2 ( 1, -1 ) * pos.zw;
    gl_Position = vec4 ( pos.xy, 0.0, 1.0 );
}

-- fragment

#version 330 core

uniform sampler2D image;
uniform sampler2D flowMap;
uniform float     time;

in  vec2 tex;
out vec4 color;

const vec2    jump         = vec2    ( 0.24, 0.20833 );
const float     flowStrength = 1;
const float    tiling       = 2;

float seesaw ( in float x )
{
    return 1 - abs ( 1 - 2 * x );
}

vec3 flowUV ( in vec2 uv, in vec2 flowVec, in vec2 jump, float tiling, float time, bool flowB )
{
    float    phaseOffset = flowB ? 0.5 : 1;
    float    progress = mod ( time + phaseOffset, 1.0 );

    return vec3 ( (uv - flowVec * progress) * tiling + vec2 ( phaseOffset ) + (time - progress) * jump, seesaw ( progress ) );
}

void main (void)
{
    vec4    flow     = texture ( flowMap, tex );
    float    noise    = flow.a;
    vec2    flowVec  = (flow.rg * 2 - vec2 ( 1.0 )) * flowStrength;
    vec3    uvA      = flowUV  ( tex, flowVec, jump, tiling, time + noise, false );
    vec3    uvB      = flowUV  ( tex, flowVec, jump, tiling, time + noise, true  );
    
    //color = (texture ( image, uvA.xy ) * uvA.z + texture ( image, uvB.xy ) * uvB.z) / (uvA.z + uvB.z);
    color = texture ( image, uvA.xy ) * uvA.z + texture ( image, uvB.xy ) * uvB.z;
}
