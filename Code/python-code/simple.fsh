#version 330 core

in vec3 p;
in vec4 c;
out vec4 color;

void main(void)
{
	//color = vec4 ( p*0.5 + vec3(0.5), 1 );	//vec4 ( 1, 1, 0, 1 );
	//color = vec4 ( abs(sin(p*5)), 1 );
	color = c;
}
