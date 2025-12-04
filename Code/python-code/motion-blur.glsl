-- vertex

#version 330 core

layout(location = 0) in vec4 pos;

out vec2 tex;

void main(void)
{
//    tex         = vec2 ( 1, -1 ) * pos.zw;
    tex = 0.5 * ( pos.xy + vec2 ( 1.0 ) );
    gl_Position = vec4 ( pos.xy, 0.0, 1.0 );
}

-- fragment

#version 330 core

uniform mat4 proj;                // we will need it in pos reconstruction
uniform mat4 mv;
uniform mat4 mvPrevInv;           // inverted model-view matrix from previous frame

uniform sampler2D colorMap;
uniform sampler2D nzMap;
//uniform sampler2D rndMap;
//uniform float     Radius;

in  vec2  tex;
out vec4  color;

const int samples  = 7;   

    // Note: it assumes proj is classical projection matrix
    // (not multiplied by smth else)
vec3    getViewPos ( in vec2 uv )
{
    vec4    nz = texture ( nzMap, uv );        // eyeZ in w
    vec2    a  = vec2    ( -2.0 / proj [0][0], -2.0 / proj [1][1] );
    vec2    b  = vec2    (  1.0 / proj [0][0],  1.0 / proj [1][1] );
    
    return nz.w * vec3 ( a*uv + b, 1.0 );
}

void    main ()
{
    vec3    pos = getViewPos ( tex );
    vec4    p0  = mvPrevInv * vec4 ( pos, 1.0 );
    vec4    p   = proj * mv * p0;
   
    //p = proj * vec4 ( pos, 1.0 );

    p.xyz /= p.w;       // perspective division
    p.xy   = 0.5 * ( p.xy + vec2 ( 1.0 ) );

    vec2    v = tex - p.xy;
    vec2    uv  = tex;
    vec4    sum = texture ( colorMap, uv );

    for ( int i = 1; i < samples; i++ )
    {
        uv  += v / 3.0;
        sum += texture ( colorMap, uv );
    }

    color = sum / samples;
}

