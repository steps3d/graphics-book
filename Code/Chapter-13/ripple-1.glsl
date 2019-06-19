-- vertex

#version 330 core

uniform mat4 mv;
uniform mat4 proj;

layout ( location = 0 ) in vec3 pos;
layout ( location = 1 ) in vec2 texCoord;

out vec2 tex;

void main(void)
{
	gl_Position = proj * mv * vec4 ( pos, 1.0 );
	tex         = texCoord;
}

-- fragment

#version 330 core

uniform sampler2D	image;

in  vec2 tex;
out vec4 color;

void main(void)
{
	color = texture ( image, tex );
}
