-- vertex

#version 420 core

uniform mat4 proj;
uniform mat4 mv;
uniform mat3 nm;
uniform	vec3 eye;		// eye position
uniform vec3 lightDir;

layout(location = 0) in vec3 pos;
layout(location = 2) in vec3 normal;

out vec3 	norm;
out vec4	color;

void main(void)
{
	vec4 	p = mv * vec4 ( pos, 1.0 );
	vec3	n = normalize ( nm * normal );
	vec3	l = normalize ( lightDir );
	
	gl_Position  = p;
	norm         = n;
	color        =  (0.2 + 0.8 * max ( 0.0, dot ( n, l ) ) ) * vec4 ( 0.7, 0.1, 0.1, 1.0 );
}

-- geometry
#version 420 core
#extension GL_EXT_gpu_shader4: enable
#extension GL_EXT_geometry_shader4: enable

layout ( triangles ) in;
layout ( line_strip, max_vertices = 128 ) out;

uniform mat4  proj;
uniform int   detail = 2;
uniform float droop  = 0.001;
uniform int   length;
uniform float step   = 0.06;
       
in  vec3  norm  [3];
in  vec4  color [3];
out vec4  clr;

void produceVertices ( in vec4 v, in vec3 n )
{
	for ( int i = 0; i <= length; i++ )
	{
		gl_Position = proj * v;
		clr         = vec4 ( 0, 1, 0, 1 );	//color [0];
		EmitVertex ();

		v.xyz += step * n;
		//v.z   -= droop * float ( i * i );
	}

	EndPrimitive ();
}

void main ()
{
	vec4 v0  = gl_PositionIn [0];
	vec4 v01 = gl_PositionIn [1] - gl_PositionIn [0];
	vec4 v02 = gl_PositionIn [2] - gl_PositionIn [0];
	vec3 n0  = normalize ( norm [0] );
	vec3 n01 = norm [1] - norm [0];
	vec3 n02 = norm [2] - norm [0];

	if ( dot ( n0, n01 ) < 0 )
		n01 = -n01;	

	if ( dot ( n0, n02 ) < 0 )
		n02 = -n02;	

	int	numLayers = 1 << detail;
	float	dt        = 1.0 / float ( numLayers );
	float	t         = 1.0;

	for ( int it = 0; it <= numLayers; it++ )
	{
		float smax = 1.0 - t;
		float ds   = smax / float ( it + 1 );

		for ( float s = 0; s < smax; s += ds )	//int is = 0; is < nums; is++ )
		{
			vec4 v = v0 + s*v01 + t*v02;
			vec3 n = normalize ( n0 + s*n01 + t*n02 );

			produceVertices ( v, n );
			//produceVertices ( s, t );
			s += ds;
		}

		t -= dt;
	}

//produceVertices ( v0 + 0.5*v01 + 0.5*v02, normalize ( n0 + 0.5*n01 + 0.5*n02 ) );

}

-- fragment
#version 420 core

in vec4 clr;

out vec4 color;

void main(void)
{
	color = clr;
}
