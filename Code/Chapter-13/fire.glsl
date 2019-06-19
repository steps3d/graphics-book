--vertex 

#version 330 core

layout ( location = 0 ) in vec4 position;

uniform mat4  proj;
uniform mat4  mv;
uniform mat3  nm;
uniform	vec3  eye;		// eye position
uniform float time;

out float phase;

const vec3  velocity = vec3 ( 0.0, 0.0, 2.0 );
const float period   = 4.0;

void main()
{
	vec3	pos = position.xyz;
	
	phase        = fract ( position.w + time / period );
	gl_Position  = proj * mv * vec4 ( pos + phase * velocity, 1.0 );
	gl_PointSize = 8.0 + 20.0 * phase;
}

--fragment

#version 330 core

uniform sampler2D sprite;
uniform sampler2D colorMap;

in float phase;
out vec4 color;

void main ()
{
	vec4	c1 = texture ( sprite, gl_PointCoord );
	vec4	c2 = texture ( colorMap, vec2 ( 1.0 - phase, 1.0 ) );
	
	color = (1.0 - phase) * vec4 ( c1.rgb * c2.rgb, 1.0 );
}
