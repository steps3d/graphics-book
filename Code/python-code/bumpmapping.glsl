-- vertex

#version 330 core

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform	vec3 eye;	// eye position
uniform vec3 light;     // light position

layout ( location = 0 ) in vec3 position;
layout ( location = 1 ) in vec2 texCoord;
layout ( location = 2 ) in vec3 normal;
layout ( location = 3 ) in vec3 tangent;
//layout ( location = 4 ) in vec3 binormal;

out vec3  n;
out vec3  v;
out vec3  l;
out vec3  h;
out vec2  tex;

void main(void)
{
	vec3	binormal = cross ( normal, tangent );
	vec4 	p = mv * vec4 ( position, 1.0 );
	vec3	n = normalize ( nm * normal );
	vec3	t = normalize ( nm * tangent );
	vec3	b = normalize ( nm * binormal );
	
	gl_Position  = proj * p;

	vec3 v1            = normalize ( eye - p.xyz );					// vector to the eye
	vec3 l1            = normalize ( light - p.xyz );
	vec3 h1            = normalize ( l1 + v1 );	

	v            = vec3 ( dot ( v1, t ), dot ( v1, b ), dot ( v1, n ) );
	l            = vec3 ( dot ( l1, t ), dot ( l1, b ), dot ( l1, n ) );
	h            = vec3 ( dot ( h1, t ), dot ( h1, b ), dot ( h1, n ) );
	tex          = texCoord * vec2 ( 1.0, 6.0 );
}

-- fragment
#version 330 core

//in  vec3 n;
in  vec3 v;
in  vec3 l;
in  vec3 h;
in  vec2 tex;
out vec4 color;

uniform sampler2D colorMap;
uniform sampler2D bumpMap;

//const vec4	diffColor = vec4 ( 0.5, 0.0, 0.0, 1.0 );
const vec4	specColor = vec4 ( 1.0 );
const float	specPower = 50.0;
	
void main(void)
{
	vec3	n    = 2.0 * texture ( bumpMap, tex ).rgb - vec3 ( 1.0 );
//vec3 n = vec3 ( 0, 0, 1 );
//n += vec3 ( 0, 0, 1 );

	vec3	v2   = normalize ( v );
	vec3	n2   = normalize ( n );
	vec3	l2   = normalize ( l );
	vec3	h2   = normalize ( h );
	vec4    clr  = texture ( colorMap, tex );

//clr = vec4 ( 0, 1, 0, 1 );

	vec4	diff = clr * max ( dot ( n2, l2 ), 0.1 );
	vec4	spec = specColor * pow ( max ( dot ( n2, h2 ), 0.0 ), specPower );
	float ka   = 0.2;
	float kd   = 0.8;
	float ks   = 0.5;

	color = (ka + kd*diff)*clr + ks*vec4(spec);

//color = vec4 ( max ( dot ( n2, l2 ), 0.1 ) );
}
