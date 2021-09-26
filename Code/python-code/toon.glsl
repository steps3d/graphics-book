--vertex

#version 330 core

layout(location = 0) in vec4 pos;

out vec2 tex;

void main(void)
{
	tex         = vec2 ( 1, -1 ) * pos.zw;
	gl_Position = vec4 ( pos.xy, 0.0, 1.0 );
}

-- fragment

#version 330 core

uniform sampler2D image;
const float 	  quantize = 16.0;
const vec4 	  lum = vec4 ( 0.2126, 0.7152, 0.0722, 0.0 );

in  vec2 tex;
out vec4 color;

void main(void)
{
	ivec2	sz   = textureSize ( image, 0 );
	float	dx   = 1.0 / float ( sz.x );
	float	dy   = 1.0 / float ( sz.y );
	vec4	clr  = texture ( image, tex );
	ivec3	iclr = ivec3   ( quantize * clr.rgb + vec3 ( 0.5 ) );
	float	c0   = dot ( lum, clr );
	float	c1   = dot ( lum, texture ( image, vec2 ( tex.x - dx, tex.y ) ) );
	float	c2   = dot ( lum, texture ( image, vec2 ( tex.x + dx, tex.y ) ) );
	float	c3   = dot ( lum, texture ( image, vec2 ( tex.x, tex.y - dy ) ) );
	float	c4   = dot ( lum, texture ( image, vec2 ( tex.x, tex.y + dy ) ) );
	float	edge = 2.0 * abs ( 4.0*c0 - c1 - c2 - c3 - c4 );

	if ( edge > 0.2 )
		color = vec4 ( 0, 0, 0, 1 );
	else			
		color = vec4 ( vec3 ( iclr ) / quantize, 1.0 );

//color = clr;
//color = vec4 ( edge );
}
