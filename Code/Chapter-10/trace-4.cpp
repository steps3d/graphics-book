#include	<stdio.h>
#include	"tracer-2.h"

class Checker : public Texture		// checkerboard texture
{
	glm::vec3	scale, offs;
	glm::vec3	color1, color2;
	
public:
	Checker ( const glm::vec3& sc, const glm::vec3& of, const glm::vec3& c1, const glm::vec3& c2 ) 
	{
		scale  = sc;
		offs   = of;
		color1 = c1;
		color2 = c2;
	}

	void apply ( HitData& hit, SurfaceData& surface ) const override
	{
		glm::vec3	p = hit.pos * scale + offs;
		int	ix = (int) ( p.x < 0 ? 1 - p.x : p.x );
		int	iy = (int) ( p.y < 0 ? 1 - p.y : p.y );
		int	iz = (int) ( p.z < 0 ? 1 - p.z : p.z );

		if ( ( ix + iy + iz ) & 1 )
			surface.color = color2;
		else
			surface.color = color1;
	}
};

int main ()
{
	Scene      * scene = new Scene;
	Plane      * p1 = new Plane  ( glm::vec3 ( 0, 0, 1 ), -7.1 );
	PointLight * l1 = new PointLight ( glm::vec3 ( 10, 5, -10 ), glm::vec3 ( 1 ) );
	Camera		 camera ( glm::vec3 ( 0, 0, -3 ), glm::vec3 ( 0, 0, 1 ), glm::vec3 ( 0, 1, 0 ), 60, 640*2, 480*2 );
	
	p1 -> defSurface.ka    = 1;
	p1 -> defSurface.kd    = 0;
	p1 -> defSurface.ks    = 0;
	p1 -> defSurface.kr    = 0;
	p1 -> defSurface.kt    = 0;
	p1 -> defSurface.color = glm::vec3 ( 0, 0, 1 );
	p1 -> setTexture ( new Checker ( glm::vec3 ( 3 ), glm::vec3 ( 0 ), glm::vec3 ( 1 ), glm::vec3 ( 0 ) ) );

	scene -> addObject ( p1 );
	scene -> addLight  ( l1 );

	for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
		{
			Sphere * s = new Sphere ( glm::vec3 ( -3 + j*2, 2.15 - i*1.45,  5 ), 0.7 );

			if ( i > 0 )
				s -> defSurface.ka = 0.2;
			else
				s -> defSurface.ka = j * 0.33;

			if ( i < 1 )
				s -> defSurface.kd = 0;
			else
			if ( i == 1 )
				s -> defSurface.kd = j * 0.33;
			else
				s -> defSurface.kd = 0.4;

			if ( i < 2 )
				s -> defSurface.ks = 0;
			else
			if ( i == 2 )
				s -> defSurface.ks = j * 0.33 / 4;
			else
				s -> defSurface.ks = 0.7;

			if ( i < 3 )
			{
				s -> defSurface.medium.nRefr = 1.0;
				s -> defSurface.kt           = 0.0;
			}
			else
			{
				s -> defSurface.medium.nRefr = 1.0 + j * 0.015;
				s -> defSurface.kt           = 0.8;
			}

			s -> defSurface.p  = 20;
			s -> defSurface.kr = 0;
			s -> defSurface.color = glm::vec3 ( 0, 1, 0 );
			s -> defSurface.medium.extinct = 0;
			s -> defSurface.glossiness = 0;

			scene -> addObject ( s );
		}
		
	camera.traceFrame ( "test-4.tga" );
	
	return 0;
}
