--vertex
#version 330 core

layout ( location = 0 ) in vec3 pos;
layout ( location = 2 ) in vec3 normal;

out vec3 n;					// normal
out vec3 v;					// vector to the eye

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform vec3 eye;

void main(void)
{
	vec4 	p = mv * vec4 ( pos, 1.0 );
	
	gl_Position  = proj * p;
	n            = normalize ( nm * normal );
	v            = normalize ( eye - p.xyz );
}

--fragment
#version 330 core

uniform samplerCube	image;

in  vec3 n;
in  vec3 v;
out vec4 color;

void main(void)
{
	color = texture ( image, 
	           reflect ( normalize(v), normalize(n) ) );
}
