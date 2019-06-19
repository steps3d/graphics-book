//
// Simple path tracer
// Author: Alexey V. Boreskov
//

#ifndef	M_PI
	#define	M_PI	3.1415926f
#endif

#include	<cassert>
#include	<algorithm>
#include	<chrono>
#include	<vector>
#include 	<glm/vec3.hpp>
#include 	<glm/vec4.hpp>
#include	<glm/geometric.hpp>
#include	"ray.h"
#include	"TgaImage.h"
#include	"randUtils.h"

#define	EPS			0.01f
#define	MAX_DEPTH	7

class Object;
class Scene;

struct	Medium				// main properties of the medium
{
	float	nRefr;			// refraction coefficient
	float	extinct;		// extinction coefficient
};

struct SurfaceData
{
	glm::vec3	pos;
	glm::vec3	n;
	glm::vec3	color;
	glm::vec3	emission;
	float		ka, kd, ks, kr, kt;
	float		p;			// power coefficient for specular
	Medium		medium;
};

struct HitData
{
	glm::vec3		pos;
	float			t;
	const Object  * object;
	bool			invertNormal;
	float			cache [13];	
};

class Object
{
public:
	SurfaceData	defSurface;
	
	Object () = default;
	virtual ~Object () = default;
	
			// check with a ray for intersection
			// cache hit data in hit
	virtual bool intersect ( const ray& r, HitData& hit ) const = 0;

			// get surface parameters at given point
	virtual void getSurface ( HitData& hit, SurfaceData& surface ) const = 0;
};

class Camera
{
	glm::vec3	pos;
	glm::vec3	view, right, up;
	int			width, height;
	float		fovx, fovy;
	TgaImage  * image;
	
public:			  
	Camera ( const glm::vec3& p, const glm::vec3& v, const glm::vec3& u, float fx, int w, int h );
	~Camera ();

	int	getWidth () const
	{
		return width;
	}
	
	int	getHeight () const
	{
		return height;
	}
	
	float	getAspect () const
	{
		return (float) width / (float) height;
	}
	
	bool traceFrame            ( const char * fileName ) const;
	bool traceFrameDistributed ( const char * fileName, int n1, int n2 ) const;
};

class Scene
{
	std::vector<Object *>		objects;
	glm::vec3					ambient = glm::vec3 ( 0.2 );
	
public:
	Scene ();
	virtual ~Scene ();
	
	virtual void addObject ( Object * object )
	{
		objects.push_back ( object );
	}
	
				// get color for ray which hits nothing
	virtual glm::vec3 getBackground ( const ray& r ) const
	{
//		return glm::vec3 ( 0, 0.5, 0.5 );
		return glm::vec3 ( 0.5f );
	}

				// find ray intersection
	virtual Object * intersect ( const ray& r, HitData& hit ) const;
	
				// trace a ray
	glm::vec3 tracePath ( const ray& r, int depth ) const;
					  				 
	static Scene * scene;
	static int		level;
	static int		rayCount;
};

class Sphere : public Object
{
	glm::vec3	center;
	float		radius, radiusSq;		// radius of sphere and squared radius
	
public:
	Sphere ( const glm::vec3& c, float r );
	
			// check with a ray for intersection
			// cache hit data in hit
	bool intersect ( const ray& r, HitData& hit ) const override;
	
			// get surface parameters at given point
	void getSurface ( HitData& hit, SurfaceData& surface ) const override;
};

class Plane : public Object
{
	glm::vec3	n;			// plane equation is (p,n) + d = 0
	float		d;
	
public:
	Plane ( const glm::vec3& nn, float dist ) : Object (), n ( nn ), d ( dist ) {}
	
			// check with a ray for intersection
			// cache hit data in hit
	bool intersect ( const ray& r, HitData& hit ) const override;
	
			// get surface parameters at given point
	void getSurface ( HitData& hit, SurfaceData& surface ) const override;
};

////////////////////////////////////////////////////////

Medium	air   = { 1, 0 };
Medium	glass = { 1.5, 0 };

Scene * Scene :: scene    = nullptr;
int		Scene :: level    = 0;
int		Scene :: rayCount = 0;

////////////////////////////// Scene ////////////////////////////////

Scene :: Scene ()
{
	scene = this;
}

Scene :: ~Scene ()
{
	for ( auto it : objects )
		delete it;
}

glm::vec3 Scene::tracePath ( const ray& r, int depth ) const
{
	HitData 	hit;
	SurfaceData	surf;
	
	if ( intersect ( r, hit ) != nullptr )
		return getBackground ( r );	
	
	if ( ++depth > MAX_DEPTH )
		return glm::vec3 ( 0 );	
	
	hit.object -> getSurface ( hit, surf );
	
	glm::vec3	c = surf.color;
	glm::vec3	n = surf.n;
	float		p = std::max ( c.x, std::max ( c.y, c.z ) );
	
	if ( randUniform () < p )	// keep the ray
		c *= 1.0f / p;
	else
		return surf.emission;
	
	glm::vec3 dir = getRandomVector ();
	
			// ensure it is in upper hemisphere
	if ( glm::dot ( n, dir ) < 0 )
		dir = -dir;
	
	glm::vec3	brdf = c / M_PI;		// diffuse BRDF

	return surf.emission + brdf * 3.1415926f * std::max ( glm::dot ( n, dir ), 0.0f ) * 
	                       tracePath ( ray ( hit.pos, dir ), depth );
}
				  
Object * Scene :: intersect ( const ray& r, HitData& hit ) const
{
	Object * closestObj  = nullptr;
	float	 closestDist = 1e10;			// assume it is far enough
	HitData	 ht;

	for ( auto obj : objects )				// check every object
		if ( obj -> intersect ( r, ht ) )	// if intersects then check distance
			if ( ht.t < closestDist )
			{
				closestDist = ht.t;
				closestObj  = obj;
				hit         = ht;
			}

	return closestObj;	
}

////////////////////////////// Camera ////////////////////////////////

Camera :: Camera ( const glm::vec3& p, const glm::vec3& v, const glm::vec3& u, float fx, int w, int h ) : pos ( p ), view ( v ), up ( u ), width ( w ), height ( h )
{
	right      = glm::cross ( view, up );
	fovx       = fx;
	fovy       = fovx * (float) height / (float) width;
	image      = new TgaImage ( width, height );
}

Camera :: ~Camera ()
{
	delete image;
}

bool Camera :: traceFrameDistributed ( const char * fileName, int n1, int n2 ) const
{
	auto 	start      = std::chrono::steady_clock::now ();
	float	tfx        = tanf ( fovx * M_PI / 180 / 2 );		// tan (fovx/2)
//	float	tfy        = tanf ( fovy * M_PI / 180 / 2 );		// tan (fovy/2)
	float	hx         = tfx * 2 / (float)width;
	float	hy         = tfx * 2 / (float)width;
	float	hxSub      = hx / n1;
	float	hySub      = hy / n2;
	int		numSamples = n1 * n2;
	
	for ( int i = 0; i < width; i++ )	
		for ( int j = 0; j < height; j++ )
		{
			float		x = (i - width/2)  * hx;
			float		y = -(j - height/2) * hy;
			float		x1 = x - 0.5f * hx;
			float		y1 = y - 0.5f * hy;
			glm::vec3	c ( 0 );
			
			for ( int iSub = 0; iSub < n1; iSub++ )
				for ( int jSub = 0; jSub < n2; jSub++ )
				{
					float	xt = x1 + hxSub * (iSub + randUniform() );
					float	yt = y1 + hySub * (jSub + randUniform() );
					
					ray		r ( pos, glm::normalize (view + xt*right + yt*up) );
					
					c += Scene::scene->tracePath ( r, 1 );
				}

			c /= numSamples;
			
			uint32_t	red   = (uint32_t)(255.0f * std::min ( 1.0f, c.x ));
			uint32_t	green = (uint32_t)(255.0f * std::min ( 1.0f, c.y ));
			uint32_t	blue  = (uint32_t)(255.0f * std::min ( 1.0f, c.z ));
			
			image -> putPixel ( i, j, image -> rgbToInt ( red, green, blue ) );
		}

	auto finish = std::chrono::steady_clock::now ();
	
	printf ( "Time to render frame %lf secs\n", std::chrono::duration_cast<std::chrono::duration<double>>(finish-start).count () );

	image -> writeToFile ( fileName );
	
	return true;
}

////////////////////////////// Geometry ////////////////////////////////

Sphere :: Sphere ( const glm::vec3& c, float r ) : Object (), center ( c )
{
	radius   = r;
	radiusSq = r * r;
}

bool	Sphere :: intersect ( const ray& r, HitData& hit ) const
{
	glm::vec3	l    = center - r.getOrigin();			// direction vector
	float		L2OC = glm::dot ( l,  l );				// squared distance
	float		tca  = glm::dot ( l, r.getDir () );		// closest dist to center
	float		t2hc = radiusSq - L2OC + tca*tca;
	float		t1, t2;

	if ( t2hc <= 0.0f )
		return false;

	t2hc = sqrtf ( t2hc );

	if ( tca < t2hc )		// we are inside
	{
		t1 = tca + t2hc;
		t2 = tca - t2hc;
	}
	else					// we are outside
	{
		t1 = tca - t2hc;
		t2 = tca + t2hc;
	}

	if ( fabs ( t1 ) < EPS )
		t1 = t2;

	if ( t1 > EPS )			// there is an intersection
	{
		hit.t         = t1;
		hit.pos       = r.pointAt ( t1 );
		hit.object    = this;
		hit.invertNormal = false;
		hit.cache [0] = t1;
		hit.cache [1] = t2;
		
		return true;
	}
	
	return false;
}

void Sphere :: getSurface ( HitData& hit, SurfaceData& surface ) const
{
	surface.pos      = hit.pos;
	surface.n        = glm::normalize ( hit.pos - center );
	surface.color    = defSurface.color;
	surface.emission = defSurface.emission;
	surface.ka       = defSurface.ka;
	surface.kd       = defSurface.kd;
	surface.ks       = defSurface.ks;
	surface.kr       = defSurface.kr;
	surface.kt       = defSurface.kt;
	surface.p        = defSurface.p;
}

bool Plane :: intersect ( const ray& r, HitData& hit ) const
{
	float	nd = glm::dot ( n, r.getDir () );

	if ( nd > -EPS && nd < EPS )		// degenerate case
		return false;

	hit.t            = -( glm::dot ( n, r.getOrigin () ) + d ) / nd;
	hit.pos          = r.pointAt ( hit.t );
	hit.object       = this;
	hit.invertNormal = false;
	hit.cache [0]    = nd;
	
	return hit.t > EPS;
}

void Plane :: getSurface ( HitData& hit, SurfaceData& surface ) const
{
	surface.pos      = hit.pos;
	surface.n        = n;
	surface.color    = defSurface.color;
	surface.emission = defSurface.emission;
	surface.ka       = defSurface.ka;
	surface.kd       = defSurface.kd;
	surface.ks       = defSurface.ks;
	surface.kr       = defSurface.kr;
	surface.kt       = defSurface.kt;
	surface.p        = defSurface.p;
}

int main ()
{
	Scene      * scene = new Scene;
	Sphere     * s1 = new Sphere ( glm::vec3 ( 0,  1, 8 ), 1.5f );
	Sphere     * s2 = new Sphere ( glm::vec3 ( -2, 0, 6 ), 0.8f );
	Plane      * p1 = new Plane  ( glm::vec3 ( 0, 1, 0 ), 1 );
//	Plane      * p2 = new Plane  ( glm::vec3 ( 0, 1, 0 ), -3 );
//	Plane      * p3 = new Plane  ( glm::vec3 ( 0, 0, 1 ), -9 );
	Camera		 camera ( glm::vec3 ( 0 ), glm::vec3 ( 0, 0, 1 ), glm::vec3 ( 0, 1, 0 ), 60, 640, 480 );
	
	s1 -> defSurface.ka     = 0.2;
	s1 -> defSurface.kd     = 0.5;
	s1 -> defSurface.ks     = 0.6;
	s1 -> defSurface.kt     = 0.0;
	s1 -> defSurface.p      = 30;
	s1 -> defSurface.kr     = 0.3;
	s1 -> defSurface.color  = glm::vec3 ( 0, 1, 0 );
	s1 -> defSurface.medium = glass;
	s1 -> defSurface.emission = glm::vec3 ( 0.3f );

	s2 -> defSurface       = s1 -> defSurface;
	s2 -> defSurface.color = glm::vec3 ( 1, 0, 0 );
	s2 -> defSurface.kr = 0;

	p1 -> defSurface       = s1 -> defSurface;
	p1 -> defSurface.emission = glm::vec3 ( 0.3f );
	p1 -> defSurface.ka    = 0.2;
	p1 -> defSurface.ks    = 0.0;
	p1 -> defSurface.kd    = 0.8;
	p1 -> defSurface.kr    = 0;
	p1 -> defSurface.color = glm::vec3 ( 1, 1, 1 );
//	p2 -> defSurface       = p1 -> defSurface;
//	p2 -> defSurface.emission = glm::vec3 ( 0.8f );
//	p3 -> defSurface       = p1 -> defSurface;
//	p3 -> defSurface.color = glm::vec3 ( 1, 1, 0 );
	
	scene -> addObject ( s1 );
	scene -> addObject ( s2 );
	scene -> addObject ( p1 );
//	scene -> addObject ( p2 );
//	scene -> addObject ( p3 );
	
	camera.traceFrameDistributed ( "path-trace.tga", 5, 5 );
	
	return 0;
}
