#include	<stdio.h>
#include	"tracer-2.h"

int main ()
{
	Scene      * scene = new Scene;
	Sphere     * s1 = new Sphere ( glm::vec3 ( 0,  1, 8 ), 1.5f );
	Sphere     * s2 = new Sphere ( glm::vec3 ( -3, 0, 8 ), 1.0f );
	Plane      * p1 = new Plane  ( glm::vec3 ( 0, 1, 0 ), 1 );
	PointLight * l1 = new PointLight ( glm::vec3 ( 10, 5, -10 ), glm::vec3 ( 1 ) );
	Camera		 camera ( glm::vec3 ( 0 ), glm::vec3 ( 0, 0, 1 ), glm::vec3 ( 0, 1, 0 ), 60, 640*2, 480*2 );
	
	s1 -> defSurface.ka     = 0.1;
	s1 -> defSurface.kd     = 0.4;
	s1 -> defSurface.ks     = 0.7;
	s1 -> defSurface.kt     = 0.0;
	s1 -> defSurface.p      = 30;
	s1 -> defSurface.kr     = 0.3;
	s1 -> defSurface.color  = glm::vec3 ( 1, 1, 0 );
	s1 -> defSurface.medium = glass;
	s1 -> defSurface.glossiness = 0;

	s2 -> defSurface       = s1 -> defSurface;
	s2 -> defSurface.color = glm::vec3 ( 1, 0, 0 );
	s2 -> defSurface.kr = 0;
	s2 -> defSurface.glossiness = 0;

	p1 -> defSurface       = s1 -> defSurface;
	p1 -> defSurface.ka    = 0.3;
	p1 -> defSurface.ks    = 0.0;
	p1 -> defSurface.kd    = 0.7;
	p1 -> defSurface.kr    = 0.5;
	p1 -> defSurface.color = glm::vec3 ( 0, 0, 1 );
	p1 -> defSurface.glossiness = 0.2;

	
	scene -> addObject ( s1 );
	scene -> addObject ( s2 );
	scene -> addObject ( p1 );
	scene -> addLight  ( l1 );
	
	camera.traceFrameDistributed ( "test-3.tga", 3, 3 );
	
	return 0;
}
