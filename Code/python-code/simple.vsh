#version 330 core

in vec3 pos;
out vec4 c;
out vec3 p;

void main(void)
{
	p = pos;
	c = vec4 ( p*0.5 + vec3(0.5), 1 );
	gl_Position = vec4(pos * vec3 ( 0.5, 1, 1 ), 1.0);
}
