-- vertex
#version 330 core

layout(location = 0) in vec3 pos;
//layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform vec3 lightDir;

out vec3 p;
out vec3 l;
out vec3 n;

void main(void)
{
	vec4 pt = mv * vec4 ( pos, 1.0 );

	p  = pos;
	n  = normal;//normalize ( nm * normal );
	l  = normalize ( lightDir );
	//v  = normalize ( eye   - p.xyz );

	gl_Position  = proj * pt;
}

-- fragment
#version 330 core

in  vec3 p;
in  vec3 l;
in  vec3 n;
out vec4 color;		// output value

uniform sampler2D image;

const vec3  lightColor = vec3 ( 1.0 );
const float gamma = 2.2;
const float pi    = 3.1415926;

	// "p" point being textured
	// "n" surface normal at "p"
	// "k" controls the sharpness of the blending in the transitions areas
vec4 biplanar( in vec3 p, in vec3 n, in float k )
{
	// grab coord derivatives for texturing
    vec3 dpdx = dFdx(p);
    vec3 dpdy = dFdy(p);

    n = abs(n);

	// determine major axis (in x; yz are following axis)
    ivec3 ma = (n.x>n.y && n.x>n.z) ? ivec3(0,1,2) :
               (n.y>n.z)            ? ivec3(1,2,0) :
                                      ivec3(2,0,1) ;

	// determine minor axis (in x; yz are following axis)
    ivec3 mi = (n.x<n.y && n.x<n.z) ? ivec3(0,1,2) :
               (n.y<n.z)            ? ivec3(1,2,0) :
                                      ivec3(2,0,1) ;

	// determine median axis (in x;  yz are following axis)
    ivec3 me = ivec3(3) - mi - ma;
    
	// project+fetch
    vec4 x = textureGrad( image, vec2(   p[ma.y],   p[ma.z]), 
                               vec2(dpdx[ma.y],dpdx[ma.z]), 
                               vec2(dpdy[ma.y],dpdy[ma.z]) );

    vec4 y = textureGrad( image, vec2(   p[me.y],   p[me.z]), 
                               vec2(dpdx[me.y],dpdx[me.z]),
                               vec2(dpdy[me.y],dpdy[me.z]) );
    
	// blend factors
    vec2 w = vec2(n[ma.x],n[me.x]);

	// make local support
    w = clamp( (w-0.5773)/(1.0-0.5773), 0.0, 1.0 );

	// shape transition
    w = pow( w, vec2(k/8.0) );

	// blend and return
    return (x*w.x + y*w.y) / (w.x + w.y);
}

void main ()
{
    color = biplanar ( p, n, 1 );
}
