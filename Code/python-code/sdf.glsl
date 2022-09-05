--vertex 

#version 330 core

layout ( location = 0 ) in vec3 position;
layout ( location = 1 ) in vec2 textureCoords;

out vec2 texCoord;

uniform mat4 proj;
uniform mat4 mv;

void main()
{
	texCoord     = textureCoords;
	gl_Position = mv * vec4 ( position, 1.0 );
}

--fragment

#version 330 core

in  vec2 texCoord;
out vec4 color;

uniform sampler2D fontMap;

void main ()
{
	float distance    = texture    ( fontMap, texCoord ).r;
	float smoothWidth = fwidth     ( distance );	
	float alpha       = smoothstep ( 0.5 - smoothWidth, 0.5 + smoothWidth, distance );
	
//	if ( alpha < 0.01 )
//		discard;

	color = vec4 ( vec3 ( alpha ), alpha );
}
