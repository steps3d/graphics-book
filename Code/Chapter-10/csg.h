//
// Basic classes to support Constructive Solid geometry (CSG)
//

#ifndef	__CSG__
#define	__CSG__

#include	"tracer.h"

struct RaySegment
{
	float	 	   t      [2];			// parameter along ray, t[0] <= t[1]
	const Object * obj    [2];			// intersected object at both ends
	bool	 	   invert [2];			// whether we should invert normal
	HitData	 	   hit    [2];
	
	float start () const
	{
		return t [0];
	}
	
	float end () const 
	{
		return t [1];
	}
	
	bool empty () const
	{
		return start () >= end ();
	}
	
	bool intersects ( const RaySegment& s ) const
	{
		return std::max ( start (), s.start () ) < std::min ( end (), s.end () );
	}
	
	void copyPoint ( const RaySegment& s, int from, int to, bool inv )
	{
		assert ( from >= 0 && from < 2 );
		assert ( to   >= 0 && to   < 2 );
		
		t      [to] = s.t      [from];
		obj    [to] = s.obj    [from];
		invert [to] = s.invert [from];
		hit    [to] = s.hit    [from];
		
		if ( inv )
			invert [to] = !invert [to];
	}
	
	void intersect ( const RaySegment& s );
	void merge     ( const RaySegment& s );
};

// Generic CSG object
//
class CsgObject : public Object
{
public:
	CsgObject () : Object () {}
	
			// get all intersections, returns false if no intersections
	virtual bool allIntersections ( const ray& r, std::vector<RaySegment>& res ) const = 0;

			// check with a ray for intersection
			// cache hit data in hit
	bool intersect ( const ray& r, HitData& hit ) const override
	{
		std::vector<RaySegment> out;

		if ( !allIntersections ( r, out ) )
			return false;
		
		if ( out.empty () )
			return false;
		
		if ( out [0].t [0] > EPS )
		{
			hit = out [0].hit [0];
			
			return true;
		}
		
		hit = out [0].hit [1];
		
		return true;
	}
};

class CompositeObject : public CsgObject
{
public:
	enum Operation
	{
		OR, 
		AND,
		SUB
	};
	
protected:
	CsgObject * op1;			// 1st operand
	CsgObject * op2;			// 2nd operand
	Operation   op;
	
public:
	CompositeObject  ( CsgObject * a, CsgObject * b, Operation oper );
	~CompositeObject () override;
	
			// get all intersections, returns false if no intersections
	bool allIntersections ( const ray& r, std::vector<RaySegment>& res ) const override;

			// check with a ray for intersection
			// cache hit data in hit
	bool intersect ( const ray& r, HitData& hit ) const override;
	
			// get surface parameters at given point
	void getSurface ( HitData& hit, SurfaceData& surface ) const override;
};

class	CsgSphere : public CsgObject
{
	glm::vec3	center;
	float		radius, radiusSq;		// radius of sphere and squared radius
	
public:
	CsgSphere ( const glm::vec3& c, float r ) : CsgObject (), center ( c )
	{
		radius   = r;
		radiusSq = r*r;
	}
	
			// get all intersections, returns false if no intersections
	bool allIntersections ( const ray& r, std::vector<RaySegment>& ) const override;

			// get surface parameters at given point
	void getSurface ( HitData& hit, SurfaceData& surface ) const override;
};

class	CsgBox : public CsgObject
{
	glm::vec3	n [3];				// normals no sides, sides are : ( p, n ) + d = 0;
	float		d1 [3], d2 [3];		// dist, for plane eq., d1 [i] < d2 [i]
	glm::vec3	center;				// center of
	glm::vec3	loc;				// origin
	glm::vec3	e1, e2, e3;			// main edges

public:
	CsgBox ( glm::vec3& l, glm::vec3& side1, glm::vec3& side2, glm::vec3& side3 );
	CsgBox ( glm::vec3& l, float a, float b, float c );

			// get all intersections, returns false if no intersections
	bool allIntersections ( const ray& r, std::vector<RaySegment>& ) const override;

			// get surface parameters at given point
	void getSurface ( HitData& hit, SurfaceData& surface ) const override;

private:
	void		initNormals ();
	glm::vec3	findNormal  ( HitData& hit ) const;
};

#endif


