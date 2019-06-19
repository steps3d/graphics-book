//
// Tracing a CSG object - sub of two spheres s1 and s2
//

#include	<stdio.h>
#include	"tracer.h"
#include	"csg.h"

int main ()
{
	Scene           * scene = new Scene;
	CsgSphere       * s1    = new CsgSphere ( glm::vec3 ( -1, 1, 10 ), 2 );
	CsgSphere       * s2    = new CsgSphere ( glm::vec3 (  0, 1,  9.5 ), 2.1 );
	CompositeObject * obj   = new CompositeObject ( s1, s2, CompositeObject::SUB );
	Plane           * p1    = new Plane  ( glm::vec3 ( 0, 1, 0 ), 1 );
	PointLight      * l1    = new PointLight ( glm::vec3 ( 10, 5, -10 ), glm::vec3 ( 1 ) );
	Camera		      camera ( glm::vec3 ( 0 ), glm::vec3 ( 0, 0, 1 ), glm::vec3 ( 0, 1, 0 ), 60, 640*2, 480*2 );
	
	s1 -> defSurface.ka     = 0.2;
	s1 -> defSurface.kd     = 0.5;
	s1 -> defSurface.ks     = 0.5;
	s1 -> defSurface.kt     = 0.0;
	s1 -> defSurface.p      = 50;
	s1 -> defSurface.kr     = 0.1;
	s1 -> defSurface.color  = glm::vec3 ( 1, 1, 0 );
	s1 -> defSurface.medium = glass;

	s2 -> defSurface       = s1 -> defSurface;
	s2 -> defSurface.color = glm::vec3 ( 1, 0, 0 );

	p1 -> defSurface       = s1 -> defSurface;
	p1 -> defSurface.ka    = 0.1;
	p1 -> defSurface.ks    = 0.4;
	p1 -> defSurface.kd    = 0.5;
	p1 -> defSurface.kr    = 0.4;
	p1 -> defSurface.color = glm::vec3 ( 0, 0, 1 );

	
	scene -> addObject ( obj );
	scene -> addObject ( p1 );
	scene -> addLight  ( l1 );
	
	camera.traceFrame ( "test-2.tga" );
	
	return 0;
}
