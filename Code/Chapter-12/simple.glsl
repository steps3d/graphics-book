-- vertex
#version 330 core

layout(location = 0) in vec3 pos;

void main(void)
{
	gl_Position = vec4 ( pos, 1.0 );
}

-- fragment
#version 330 core

out vec4 			color;		// output value

void main ()
{
	color = vec4 ( 1, 1, 0, 1 );
}