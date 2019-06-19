-- vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1 ) in vec2 tex;
layout(location = 2) in vec3 normal;

uniform mat4 proj;
uniform mat4 mv;
uniform mat4 projMat;

out vec2 tx;
out vec4 projCoord;

void main(void)
{
	tx          = tex;
	projCoord   = projMat * mv * vec4 ( pos, 1.0 );
	gl_Position = proj * mv * vec4 ( pos, 1.0 );
}

-- fragment
#version 330 core

in vec2 tx;
in vec4 projCoord;

out vec4 color;		// output value

uniform sampler2D image;
uniform sampler2D projMap;

void main ()
{
	vec4 c1 = texture ( image, tx );
	vec4 c2 = textureProj ( projMap, projCoord );

	color = 0.5*c1 + 0.5*c2;
}