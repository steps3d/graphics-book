-- vertex

#version 330 core

layout(location = 0) in vec4 pos;

out vec2 tex;

void main(void)
{
	tex     	= vec2 ( 1, -1 ) * pos.zw;
	gl_Position = vec4 ( pos.xy, 0.0, 1.0 );
}

-- fragment

#version 330 core

uniform float 	time;
in  vec2 tex;
out vec4 color;

uniform float _scale = 6.0;
uniform float _scaleStep = 1.2;
uniform float _rotationStep = 5;
uniform int _iterations = 16;
uniform float _uvAnimationSpeed = 3.5;
uniform float _rippleStrength = 0.9;
uniform float _rippleMaxFrequency = 1.4;
uniform float _rippleSpeed = 5;
uniform float _brightness = 2;

    // Get 2D rotation matrix given rotation in degrees.
mat2 get2DRotationMatrix ( float angle )
{
	float a = angle;// * 3.1415926 / 180.0;
	float c = cos ( a );
	float s = sin ( a );

	return mat2 ( c, s, -s, c );
}

	// Output this function directly (default values only for reference).
float f ( float scale, float scaleMultStep, float rotationStep, int iterations,
      	vec2 uv, float uvAnimationSpeed, float rippleStrength, float rippleMaxFrequency,
      	float rippleSpeed, float brightness )
{
   				 // Remap to [-1.0, 1.0].
	uv = vec2 ( uv - vec2(0.5) ) * 2.0;

	vec2    n, q;
	float    invertedRadialGradient = pow(length(uv), 2.0);   	 // ??????, mb dot(uv,uv)
	float    fractal = 0.0;
	mat2     rotationMatrix = get2DRotationMatrix(rotationStep);
	float    t = time;
	float    uvTime = t * uvAnimationSpeed;

            	// Ripples can be pre-calculated and passed from outside.
            	// They don't need to be here in this function.
 	float ripples = sin ( t * rippleSpeed - invertedRadialGradient * rippleMaxFrequency ) * rippleStrength;

 	for ( int i = 0; i < iterations; i++ )
 	{
     	uv = rotationMatrix * uv;
     	n  = rotationMatrix * n;

     	vec2 animatedUV = (uv * scale) + uvTime;

     	q   	= animatedUV + ripples + i + n;
     	fractal += dot(cos(q) / scale, vec2(1.0, 1.0) * brightness);
     	n  	-= sin(q);
     	scale  *= scaleMultStep;
 	}

  	return fractal;
}


void main (void)
{
	float c = f ( _scale, _scaleStep,
            	_rotationStep, _iterations,
            	tex, _uvAnimationSpeed,
            	_rippleStrength, _rippleMaxFrequency, _rippleSpeed, _brightness );

	color = vec4 ( 0.5*(c + 1));
}
