-- vertex

#version 330 core

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform	vec3 eye;
uniform vec3 light;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec3 n;
out vec3 v;
out vec3 l;
out	vec3 h;
out vec2 tex;
out vec3 pp;

void main(void)
{
	vec4	p = mv * vec4 ( pos, 1.0 );

	v           = normalize ( eye - p.xyz );
	n           = normalize ( nm * normal );
	l           = normalize ( light - p.xyz );
	h           = normalize ( l + v );	
	tex         = texCoord;
	pp          = pos.xyz * 0.234567;
	gl_Position = proj * p;
}

-- fragment

#version 330 core

//
// Burn fragment shader
//

uniform sampler3D 	noiseMap;
uniform sampler2D	burnMap;
uniform sampler2D	decalMap;
uniform float		time;
uniform	float		offs;
uniform	float		scale;

in vec3 n;
in vec3 v;
in vec3 l;
in vec3 h;
in vec2 tex;
in vec3 pp;

out vec4 color;

vec3	turbulence ( const in vec3 p, const in float freqScale )
{
	float	sum = 0.0;
	vec3	t   = vec3 ( 0.0 );
	float	f   = 1.0;

	for ( int i = 0; i <= 3; i++ )
	{
		t   += abs ( 2.0 * texture ( noiseMap, f * p.xyz ).rgb - vec3 ( 1.0 ) ) / f;
		sum += 1.0 / f;
		f   *= freqScale;
	}

										// remap from [0,sum] to [-1,1]
	return 2.0 * t / sum - vec3 ( 1.0 );
}

float saturate ( float v )
{
	return max ( min ( 1.0, v ), 0.0 );
}

float	animate ()
{
	const float period = 8.0;
	
	return fract ( time / period );
}

void main (void)
{
	const float delta     = 0.3;

	vec3	ns     = abs( turbulence ( 0.8 * pp, 2.07193 ));
	float	t      = animate ();
	vec4	clr    = texture ( decalMap, tex );	//0.5 * vec4 ( 1.0, 1.0, 1.0, 1.0 );		// basic color
	float   factor = 0.5 * ns.x + 1.0 * pp.y - offs - scale * t;

	float v         = 1.1 - abs ( factor - 0.45 ) / 0.1;
	float emberGlow = saturate ( v );
	float v2        = (factor - 0.5) / 0.2 + 0.5;
	float charring  = saturate ( v2 );

	//vec4	burn = texture ( burnMap, vec2 ( min ( 0.1, factor ), 0.5 ) );
	vec3	n2   = normalize ( n );
	vec3	l2   = normalize ( l );
	float	diff = max ( dot ( n2, l2 ), 0.0 ) + 0.3;

	if ( v2 <= -0.4 )
		discard;

	vec4	glowColor;

	if ( emberGlow <= 0.7 )
		glowColor = mix ( vec4 ( 1, 0, 0, 1 ), vec4 ( 1, 1, 0, 1 ), emberGlow / 0.7 );
	else 
		glowColor = mix ( vec4 ( 1, 1, 0, 1 ), vec4 ( 1, 1, 0.5, 1 ), ( emberGlow - 0.7 ) / 0.3 );

	color = 4 * emberGlow * glowColor + clr * diff * charring;
}
