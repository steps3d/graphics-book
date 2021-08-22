-- compute
#version 430

layout( local_size_x = 1000 ) in;

uniform float gravity1        = 1000.0;
uniform vec3  blackHolePos1   = vec3(5,0,0);
uniform float gravity2        = 1000.0;
uniform vec3  blackHolePos2   = vec3(-5,0,0);
uniform float particleInvMass = 1.0 / 0.1;
uniform float deltaT          = 0.0005;
uniform float maxDist         = 45.0;

layout(std430, binding = 0) buffer Pos 
{
	vec4 position [];
};

layout(std430, binding = 1) buffer Vel 
{
	vec4 velocity [];
};

void main() 
{
	uint idx = gl_GlobalInvocationID.x;
	vec3 p   = position [idx].xyz;

				// force from black hole #1
	vec3	d     = blackHolePos1 - p;
	float	dist  = length(d);
	vec3	force = (gravity1 / dist) * normalize(d);

				// force from black hole #2
	d      = blackHolePos2 - p;
	dist   = length(d);
	force += (gravity2 / dist) * normalize(d);

				// reset particles that get too far from the attractors
	if ( dist > maxDist ) 
		position [idx] = vec4(0,0,0,1);
	else 
	{
				// apply simple Euler integrator
		vec3 a = force * particleInvMass;

		position [idx] = vec4( p + velocity[idx].xyz * deltaT + 0.5 * a * deltaT * deltaT, 1.0 );
		velocity [idx] = vec4( velocity[idx].xyz + a * deltaT, 0.0 );
	}
}
