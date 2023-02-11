-- vertex
#version 330 core

layout(location = 0) in vec3 pos;
//layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform vec3 lightDir;

out vec3 p;
out vec3 l;
out vec3 n;

void main(void)
{
	vec4 pt = mv * vec4 ( pos, 1.0 );

	p  = pos;
	n  = normal;//normalize ( nm * normal );
	l  = normalize ( lightDir );
	//v  = normalize ( eye   - p.xyz );

	gl_Position  = proj * pt;
}

-- fragment
#version 330 core

in  vec3 p;
in  vec3 l;
in  vec3 n;
out vec4 color;		// output value

uniform sampler2D image;

const vec3  lightColor = vec3 ( 1.0 );
const float gamma = 2.2;
const float pi    = 3.1415926;

void main ()
{
    color = texture ( image, p.yz );
}
