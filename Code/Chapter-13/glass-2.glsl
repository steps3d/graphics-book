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

void main(void)
{
	vec4 	p = mv * vec4 ( position, 1.0 );
	
	gl_Position  = proj * p;
	n            = normalize ( nm * normal );
	v            = normalize ( eye - p.xyz );					// vector to the eye
}

-- fragment

#version 330 core

in	vec3 n;
in	vec3 v;
out vec4 color;

uniform samplerCube cubeMap;

void main(void)
{
	const vec3	eta = vec3 ( 0.9, 0.92, 0.94 );

	vec3  v2 = normalize ( v );
	vec3  n2 = normalize ( n );
	vec3  r  = reflect ( v2, n2 );
	vec3  tRed   = refract ( -v2, n2, eta.r );
	vec3  tGreen = refract ( -v2, n2, eta.g );
	vec3  tBlue  = refract ( -v2, n2, eta.b );
    vec3  refractionColorRed   = texture ( cubeMap, tRed   ).rgb;
    vec3  refractionColorGreen = texture ( cubeMap, tGreen ).rgb;
    vec3  refractionColorBlue  = texture ( cubeMap, tBlue  ).rgb;
	vec3  refractionColor      = vec3 ( refractionColorRed.r,
                                        refractionColorGreen.g,
                                        refractionColorBlue.b );
	
	float f  = pow ( max ( 0, dot ( v2, n2 ) ), 5.0 );

	color = texture( cubeMap, r )*(1.0-f) + vec4 ( refractionColor, 1.0 )*f;
}
