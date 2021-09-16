--vertex
#version 330 core

layout ( location = 0 ) in vec3 pos;
layout ( location = 1 ) in vec2 tex;
out vec2  tx;

uniform mat4 proj;
uniform mat4 mv;

void main(void)
{
	gl_Position = proj * mv * vec4(pos, 1.0);
	tx          = tex;	
}

--fragment
#version 330 core

uniform sampler2D image;
in  vec2 tx;
out vec4 color;

void main(void)
{
	color = texture ( image, tx*vec2(1,1) );
}
