-- vertex

#version 330 core

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform	vec3 eye;		// eye position
uniform vec3 lightDir;

layout(location = 0) in vec3 pos;
layout(location = 2) in vec3 normal;

out vec3  n;
out vec3  v;
out vec3  l;

void main(void)
{
	vec4 	p = mv * vec4 ( pos, 1.0 );
	
	gl_Position  = proj * p;
	n            = normalize ( nm * normal );
	v            = normalize ( eye - p.xyz );					// vector to the eye
	l            = normalize ( lightDir );
}

-- fragment
#version 330 core

in vec3 v;
in vec3 l;
in vec3 n;

out vec4 color;

void main( void )
{
  vec4  clr  = vec4 ( 0.7, 0.1, 0.1, 1.0 );
  float roughness = 1.0; //0.58;
  float r2        = roughness * roughness; 
  vec3  l2        = normalize( l );
  vec3  n2        = normalize( n );
  vec3  v2        = normalize( v );
  float nl        = dot( n2, l2 );
  float nv        = dot( n2, v2 );
  float c         = sqrt ((1.0 - nl*nl) * (1.0 - nv * nv) ) / max ( nl, nv );
  float gamma     = -dot ( normalize ( v2 - n2* nv ), normalize ( l2 - n2*nl ) );
  float a         = 1.0 - 0.5 * r2 / (r2 + 0.33);
  float b         = 0.45 * r2 / (r2 + 0.09);  
   
  color = max(nl, 0.0) * (a + b * max ( 0.0, gamma) * c) * clr;
}
