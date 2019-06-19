#ifndef	__RAY_TRACER__
#define __RAY_TRACER__

#ifndef	M_PI
	#define	M_PI	3.1415926f
#endif

#include	<algorithm>
#include	<cassert>
#include	<memory>
#include	<vector>
#include 	<glm/vec3.hpp>
#include 	<glm/vec4.hpp>
#include	<glm/geometric.hpp>
#include	"ray.h"
#include	"TgaImage.h"

#define	EPS			0.01f
#define	MAX_DEPTH	7

class Object;
class LightSource;
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

class LightSource
{
public:
	glm::vec3	color;
	
	explicit LightSource ( const glm::vec3& c ) : color ( c ) {}
	virtual ~LightSource () = default;
	
				// check whether it is in shadow ????
	virtual float shadow ( const glm::vec3& pt,  glm::vec3& l ) const = 0;
};

class	PointLight : public LightSource
{
	glm::vec3	pos;
	
public:
	PointLight ( const glm::vec3& p, const glm::vec3& c ) : LightSource ( c ), pos ( p ) {}
	
				// check whether it is in shadow ????
	float shadow ( const glm::vec3& pt, glm::vec3& l ) const override;
};

class Camera
{
	glm::vec3	pos;
	glm::vec3	view, right, up;
	int			width, height;
	float		fovx, fovy;
	std::unique_ptr<TgaImage> image;
	
public:			  
	Camera ( const glm::vec3& p, const glm::vec3& v, const glm::vec3& u, float fx, int w, int h );
	~Camera () = default;

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
	std::vector<LightSource *>  lights;
	std::vector<Object *>			objects;
	glm::vec3					ambient = glm::vec3 ( 0.2 );
	
public:
	Scene ();
	virtual ~Scene ()
	{
		for ( auto it : objects )
			delete it;

		for ( auto it : lights )
			delete it;
	}
	
	virtual void addObject ( Object * object )
	{
		objects.push_back ( object );
	}
	
	virtual void addLight  ( LightSource * light )
	{
		lights.push_back ( light );
	}
	
				// get color for ray which hits nothing
	virtual glm::vec3 getBackground ( const ray& r ) const
	{
		return glm::vec3 ( 0, 0.5, 0.5 );
	}

				// find ray intersection
	virtual Object * intersect ( const ray& r, HitData& hit ) const;
	
				// trace a ray
	glm::vec3 trace ( const Medium& curMedium, float weight, int depth,
	                  const ray& r ) const;
					  
				// get color
	glm::vec3 shade ( const Medium& curMedium, float weight, int depth,
	                  const glm::vec3& p, const glm::vec3& v,
					  HitData& hit ) const;
					 
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


class Tri : public Object
{
	glm::vec3	p [3];;
	glm::vec3	normal;
	
public:
	Tri ( const glm::vec3& a, const glm::vec3& b, const glm::vec3& c );
	
			// check with a ray for intersection
			// cache hit data in hit
	bool intersect ( const ray& r, HitData& hit ) const override;
};

class Box : public Object
{
//	glm::vec3	center;
//	float		radius, rediusSq;
	
public:
	Box ( /*const glm::vec3& c, float r*/ );
	
			// check with a ray for intersection
			// cache hit data in hit
	bool intersect ( const ray& r, HitData& hit ) const override;
};

inline float rnd ()
{
	return (float)rand() / (float)RAND_MAX;
}

inline glm::vec3 sphereRnd ()
{
	float     t = 2.0f * M_PI * rnd ();	// случайное число, равномерно распределенное в [0,2*pi]
	float     z = 2.0f * rnd () - 1.0f;	// случайное число, равномерно распределенное в [-1,1]
	float     r = sqrtf ( 1.0f - z*z );
	
	return glm::vec3 ( r * cos ( t ), r * sin ( t ), z );
}

extern Medium glass;
extern Medium air;

#endif
