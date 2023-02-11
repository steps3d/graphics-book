-- vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 2) in vec3 normal;

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform vec3 lightDir;

out vec3 p;
out vec3 l;
out vec3 n;
out vec3 nw;

void main(void)
{
	vec4 pt = mv * vec4 ( pos, 1.0 );

	p  = pos;
	nw = normal;
	n  = normalize ( nm * normal );
	l  = normalize ( nm * lightDir );

	gl_Position  = proj * pt;
}

-- fragment
#version 330 core

in  vec3 p;
in  vec3 l;
in  vec3 n;
in  vec3 nw;
out vec4 color;		// output value

uniform sampler2D image;

vec3 unpackNormal ( in vec3 n )
{
    return 2 * n - vec3 ( 1.0 );
}

	// "p" point being textured
	// "k" controls the sharpness of the blending in the transitions areas
vec3 boxmap ( in vec3 p, in float k )
{
		// blending coefficients
    vec3 blend = pow ( abs ( nw ), vec3 ( k ) );
	
    blend /= blend.x + blend.y + blend.z;	// note: they all non-negative due to abs
	
    vec2 uvX = p.zy; 		// x facing plane
    vec2 uvY = p.xz; 		// y facing plane
    vec2 uvZ = p.xy; 		// z facing plane

	// tangent space normal maps
    vec3 tnormalX = unpackNormal ( texture ( image, uvX ).rgb );
    vec3 tnormalY = unpackNormal ( texture ( image, uvY ).rgb );
    vec3 tnormalZ = unpackNormal ( texture ( image, uvZ ).rgb );

	// swizzle world normals into tangent space and apply whiteout blend
    tnormalX = vec3 ( tnormalX.xy + nw.zy, abs(tnormalX.z) * nw.x );
    tnormalY = vec3 ( tnormalY.xy + nw.xz, abs(tnormalY.z) * nw.y );
    tnormalZ = vec3 ( tnormalZ.xy + nw.xy, abs(tnormalZ.z) * nw.z );

	// Swizzle tangent normals to match world orientation and triblend
    return normalize ( tnormalX.zyx * blend.x + tnormalY.xzy * blend.y + tnormalZ.xyz * blend.z );
}

void main ()
{
    color = vec4 ( 0.5 * ( 1 + dot ( l, boxmap ( p * 0.25, 1 ) ) ) );
}
