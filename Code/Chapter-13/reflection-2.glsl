-- vertex

#version 330 core

uniform mat4 mv;
uniform mat4 proj;
uniform mat4 mirrorMat;

layout ( location = 0 ) in vec3 position;

out vec4 pos;

void main(void)
{
	gl_Position = proj * mv * vec4 ( position, 1.0 );
	pos         = mirrorMat * vec4 ( position, 1.0 );
}

-- fragment

#version 330 core

uniform sampler2D image;
uniform float     time;

in  vec4 pos;
out vec4 color;

void main(void)
{
//	vec3	ncoord    = vec3 ( pos.x, pos.y + 0.1*time, pos.z ) * 3.1;

	color = 0.7*texture(image, pos.xy/pos.w);
}
