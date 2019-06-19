-- vertex

#version 330 core

layout ( location = 0 ) in vec3 position;
layout ( location = 1 ) in vec2 texCoord;
layout ( location = 2 ) in vec3 normal;
layout ( location = 3 ) in vec3 tangent;
layout ( location = 4 ) in vec3 binormal;

uniform mat4 mv;
uniform mat4 proj;
uniform mat3 nm;

uniform  float	  tl;
uniform	 vec3	  light;
uniform	 vec3	  eye;
uniform  float    time;

out vec3 lt;
out vec3 ht;
out vec3 furDir;
out vec2 tex;

void main ()
{
	vec3  pp = position + 0.5 * tl * normal;
	float d1 = 0.01 * sin ( 0.731*time ) * tl;
	float d2 = 0.03 * sin ( 0.231*time ) * tl;
	
	vec3	p = (mv * vec4 ( position, 1.0 ) ).xyz;		// transformed point to world space
	vec3	l = normalize ( light - p );				// vector to light source
	vec3	v = normalize ( eye   - p );				// vector to the eye
	vec3	h = l + v;
	vec3	n = nm * normal;	// transformed n
	vec3 	t = nm * tangent;	// transformed t
	vec3 	b = nm * binormal;	// transformed b

	lt     = vec3 ( dot ( l, t ), dot ( l, b ), dot ( l, n ) );
	ht     = vec3 ( dot ( h, t ), dot ( h, b ), dot ( h, n ) );
	furDir = vec3 ( d1, d2, 1.0 );
	tex    = 2.25*texCoord + vec2 ( d1, d2 );
	
	gl_Position = proj * mv * vec4 ( pp, 1.0 );
}

-- fragment

#version 330 core

uniform sampler2D furMap;

uniform float tl;
uniform vec3  light;
uniform float time;

in vec2 tex;
in vec3 lt;
in vec3 ht;
in vec3 furDir;

out vec4 color;

void main () 
{
	vec4 c = texture ( furMap, 0.25*tex * vec2 ( 6.0, 12.0 ) );
	
	if ( c.a < 0.02 )
		discard;
		
	vec3	t    = normalize ( furDir );
	float	tl   = dot       ( normalize ( lt ), t );
	float	th   = dot       ( normalize ( ht ), t );
	float	spec = pow       ( 1.0 - th*th, 70.0 );
	
	color = vec4 ( c.rgb*sqrt(1.0-tl*tl) + c.rgb*vec3(spec),  c.a*0.1 ); 
}
