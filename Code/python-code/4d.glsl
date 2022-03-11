-- vertex
#version 330 core

uniform float sinA1;
uniform float cosA1;
uniform float sinA2;    
uniform float cosA2;    
uniform vec4  e1;
uniform vec4  e2;

layout(location = 0) in vec4 pos;

void main(void)
{
    mat4    m = mat4 ( 0.0 );
    
    m [0][0] =  cosA1;
    m [1][1] =  cosA1;
    m [0][1] =  sinA1;
    m [1][0] = -sinA1;
    m [2][2] =  cosA2;
    m [3][3] =  cosA2;
    m [2][3] =  sinA2;
    m [3][2] = -sinA2;
    m [3][3] = 1.0;
    
    vec4    p = 0.7 * m * ( pos - vec4 ( 0.5 ) );
    
    gl_Position  = vec4 ( dot ( p, e1 ), dot ( p, e2 ), 0, 1 );
}

-- fragment
#version 330 core

uniform vec4  color;

out vec4 clr;

void main(void)
{
    clr = color;
}
