-- vertex

#version 330 core

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform	vec3 eye;		// eye position
uniform vec3 lightDir;

layout(location = 0) in vec3 pos;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 binormal;

out vec3 l;
out vec3 h;
out vec3 v;
out vec3 t;
out vec3 b;
out vec3 n;

void main(void)
{
	vec4 p = mv * vec4 ( pos, 1.0 );

	gl_Position  = proj * p;

	n = normalize(nm * normal);
	t = normalize(nm * tangent);
	b = normalize(nm * binormal);
	l = normalize(lightDir);
	v = normalize(eye   - p.xyz);
	h = normalize(l + v);
}

-- fragment
#version 330 core

in vec3 l;
in vec3 h;
in vec3 v;
in vec3 t;
in vec3 b;
in vec3 n;

out vec4 color;

uniform float mx, my;
uniform vec4  ks, kd;
uniform float r0;
uniform vec4  specular;
uniform vec4  lightColor;

#define PI 3.1415926

void main( void )
{  
	vec3  n2   = normalize( n );
	vec3  t2   = normalize( t );
	vec3  b2   = normalize( b );
	vec3  l2   = normalize( l );
	vec3  h2   = normalize( h );
	vec3  v2   = normalize( v ); 
	float nv   = max(0.0, dot(n2, v2));
	float nl   = max(0.0, dot(n2, l2));
	float nh   = max(0.0, dot(n2, h2));
	float hl   = max(0.0, dot(h2, l2));
	float t1h  = dot(b2, h2 );
	float t2h  = dot(t2, h2 );

				  // calculate diffuse
	float   rd = (28.0/(23.0*PI)) * 
			     (1.0 - pow(1.0 - 0.5*nv, 5.0)) * 
			     (1.0 - pow(1.0 - 0.5*nl, 5.0));

				  // calculate specular
	float   A  = sqrt((mx + 1.0) * (my + 1.0)) / (8.0 * PI); 
	float   B  = pow(nh, (mx*t1h*t1h + my*t2h*t2h)/ (1.0 - nh*nh));
	float   F  = (r0 + (1.0 - r0) * pow(1.0 - hl, 5.0));
	float   rs = A * B * F / (hl * max (nl, nv));

	color = lightColor * nl * ( kd * ( 1.0 - ks ) * rd + ks * rs );
}
