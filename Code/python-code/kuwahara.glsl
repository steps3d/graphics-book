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
uniform int 	  radius;

in  vec2 tex;
out vec4 color;

vec4 computeMeanAndVariance ( ivec4 range )
{
	ivec2	sz   = textureSize ( image, 0 );
	float	n    = float (( range.z - range.x + 1) * ( range.w - range.y + 1));
	vec3	mean = vec3 ( 0.0 );
	vec3	var  = vec3 ( 0.0 );

	for ( int x = range.x; x <= range.z; x++ )
		for ( int y = range.y; y <= range.w; y++ )
		{
			vec3 clr = texture ( image , tex + vec2 ( x, y ) / sz ).rgb;
			
			mean += clr;
			var  += clr * clr;
		}
		
	mean /= n;
	var   = var / n - mean * mean;
	
	return vec4 ( mean.rgb, var.r + var.g + var.z );
}

void main(void)
{
	vec4	mv [4];		// mean in rgb, variance in a
	
	mv [0] = computeMeanAndVariance ( ivec4 ( -radius , -radius , 0,       0 ) );
	mv [1] = computeMeanAndVariance ( ivec4 ( 0,        -radius , radius , 0 ) );
	mv [2] = computeMeanAndVariance ( ivec4 ( 0,        0,        radius , radius ) );
	mv [3] = computeMeanAndVariance ( ivec4 ( -radius , 0,        0,       radius ) );
	
	float	minVar = mv [0].a;
	vec3	clr    = mv [0].rgb;
	
	for ( int i = 1; i < 4; i++ )
		if ( mv [i].a < minVar )
		{
			minVar = mv [i].a;
			clr    = mv [i].rgb;
		}
		
	color = vec4 ( clr, 1.0 );
}
