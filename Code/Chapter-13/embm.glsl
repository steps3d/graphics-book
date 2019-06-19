-- vertex

#version 330 core

layout ( location = 0 ) in vec3 position;
layout ( location = 1 ) in vec2 texCoord;
layout ( location = 2 ) in vec3 normal;
layout ( location = 3 ) in vec3 tangent;
layout ( location = 4 ) in vec3 binormal;

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform	vec3 eye;		// eye position

out vec3  n;
out vec3  v;
out vec3  l;
out vec3  h;
out vec2  tex;

void main(void)
{
	vec4 	p = mv * vec4 ( position, 1.0 );
	vec3	n = normalize ( nm * normal );
	vec3	t = normalize ( nm * tangent );
	vec3	b = normalize ( nm * binormal );
	
	gl_Position  = proj * p;
	n            = normalize ( nm * normal );
	v            = normalize ( eye - p.xyz );					// vector to the eye
	v            = vec3 ( dot ( v, t ), dot ( v, b ), dot ( v, n ) );
	tex          = texCoord * vec2 ( 1.0, 6.0 );
}

-- fragment

#version 330 core

in  vec3 n;
in  vec3 v;
in  vec2 tex;
out vec4 color;

uniform sampler2D   bumpMap;
uniform samplerCube cubeMap;

void main(void)
{
	const vec4	diffColor = vec4 ( 0.5, 0.0, 0.0, 1.0 );
	const vec4	specColor = vec4 ( 1.0 );
	const float	specPower = 30.0;

				// читаем нормаль и переводим из цвета в [-1,1]^3
	vec3	n    = 2.0 * (texture ( bumpMap, tex ).rgb - vec3 ( 0.5 ));
	vec3	v2   = normalize ( v );
	vec3	n2   = normalize ( n );
	vec3    r    = reflect ( v2, n2 );

	color = texture ( cubeMap, r );
}
