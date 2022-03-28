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

vec2 flowUV ( in vec2 uv, in vec2 flowVec, float time )
{
    return uv - flowVec * time;
}

void main (void)
{
    vec2    flowVec = texture ( flowMap, tex ).rg * 2 - vec2 ( 1.0 );
    vec2    uv      = flowUV  ( tex, flowVec, time );
    
    color = texture ( image, uv );
}
