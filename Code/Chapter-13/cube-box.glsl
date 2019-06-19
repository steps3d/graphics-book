-- vertex

#version 330 core

layout ( location = 0 ) in vec3 position;

uniform mat4 proj;
uniform mat4 mv;
uniform	vec3 eye;		// eye position

out vec3  v;

void main(void)
{
	vec4 	p = mv * vec4 ( position, 1.0 );
	
	gl_Position  = proj * p;
	v            = normalize ( eye - p.xyz );					// vector to the eye
}

-- fragment

#version 330 core

in  vec3 v;
out vec4 color;

uniform samplerCube cubeMap;

void main(void)
{
	color = texture ( cubeMap, -v );
}
