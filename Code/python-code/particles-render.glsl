-- vertex
#version 430

layout (location = 0) in vec4 pos;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * pos;
}

-- fragment
#version 430

layout( location = 0 ) out vec4 fragColor;

void main() 
{
    fragColor = vec4 ( 1.0, 1.0, 0.0, 0.1 );
}
