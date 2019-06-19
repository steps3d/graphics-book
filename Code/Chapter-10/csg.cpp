//
// Basic classes to support Constructive Solid geometry (CSG)
//

#include	"csg.h"

void csgAnd ( const std::vector<RaySegment>& a, const std::vector<RaySegment>& b, 
              std::vector<RaySegment>& c );
void csgOr  ( const std::vector<RaySegment>& a, const std::vector<RaySegment>& b, 
              std::vector<RaySegment>& c );
void csgSub ( const std::vector<RaySegment>& a, const std::vector<RaySegment>& b, 
              std::vector<RaySegment>& c );

CompositeObject :: CompositeObject  ( CsgObject * a, CsgObject * b, Operation oper ) : CsgObject () 
{
	op1 = a;
	op2 = b;
	op  = oper;
}

CompositeObject :: ~CompositeObject ()
{
	delete op1;
	delete op2;
}

bool CompositeObject :: intersect ( const ray& r, HitData& hit ) const
{
	std::vector<RaySegment> out;
	int						index = 0;

	if ( !allIntersections ( r, out ) )
		return false;
	
	if ( out.empty () )
		return false;
	
	if ( out [0].t [0] < EPS )
		index = 1;
	
	if ( out[0].t[index] <= EPS )
		return false;

	hit              = out [0].hit    [index];
	hit.invertNormal = out [0].invert [index];
	
	return true;
}

			// get surface parameters at given point
void CompositeObject :: getSurface ( HitData& hit, SurfaceData& surface ) const
{
									// delegate getting surface parameters
	hit.object -> getSurface ( hit, surface );
	
	if ( hit.invertNormal )
		surface.n = -surface.n;
}

bool	CompositeObject :: allIntersections ( const ray& r, std::vector<RaySegment>& res ) const
{
	std::vector<RaySegment>	l1, l2;
	
	if ( op1 != nullptr )
		op1->allIntersections ( r, l1 );
	
	if ( op2 != nullptr )
		op2->allIntersections ( r, l2 );
	
	if ( op == OR )
		csgOr ( l1, l2, res );
	else if ( op == AND )
		csgAnd ( l1, l2, res );
	else if ( op == SUB )
		csgSub ( l1, l2, res );
	
	return !res.empty ();
}

/////////////////////////// CsgSphere //////////////////////////

bool CsgSphere :: allIntersections ( const ray& r, std::vector<RaySegment>& res ) const
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

	if ( t1 < EPS && t2 < EPS )
		return false;
	
	if ( t1 > t2 )
		std::swap ( t1, t2 );
	
	RaySegment	seg;
	
	seg.t      [0] = t1;
	seg.t      [1] = t2;
	seg.obj    [0] = this;
	seg.obj    [1] = this;
	seg.invert [0] = false;
	seg.invert [1] = false;
	seg.hit    [0].t = t1;
	seg.hit    [1].t = t2;
	seg.hit    [0].pos = r.pointAt ( t1 );
	seg.hit    [1].pos = r.pointAt ( t2 );
	seg.hit    [0].object = this;
	seg.hit    [1].object = this;
	seg.hit    [0].cache [0] = t1;
	seg.hit    [0].cache [1] = t2;
	seg.hit    [1].cache [0] = t1;
	seg.hit    [1].cache [1] = t2;

	res.push_back ( seg );
		
	return true;
}

void CsgSphere :: getSurface ( HitData& hit, SurfaceData& surface ) const
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
}

/////////////////////////// CsgBox //////////////////////////

CsgBox :: CsgBox ( glm::vec3& l, glm::vec3& side1, glm::vec3& side2, glm::vec3& side3 ) : CsgObject ()
{
	loc    = l;
	e1     = side1;
	e2     = side2;
	e3     = side3;
	center = loc + ( e1 + e2 + e3 ) * 0.5f;
	
	initNormals ();
}
CsgBox :: CsgBox ( glm::vec3& l, float a, float b, float c ) : CsgObject ()
{
	loc    = l;
	e1     = glm::vec3 ( a, 0, 0 );
	e2     = glm::vec3 ( 0, b, 0 );
	e3     = glm::vec3 ( 0, 0, c );
	center = loc + ( e1 + e2 + e3 ) * 0.5f;
	
	initNormals ();
}

void	CsgBox::initNormals ()
{
	n  [0] = glm::normalize ( glm::cross ( e1, e2 ) );
	d1 [0] = -glm::dot ( n [0], loc );
	d2 [0] = -glm::dot ( n [0], loc + e3 );

	n  [1] = glm::normalize ( glm::cross ( e1, e3 ) );
	d1 [1] = -glm::dot ( n [1], loc );
	d2 [1] = -glm::dot ( n [1], loc + e2 );

	n  [2] = glm::normalize ( glm::cross ( e2, e3 ) );
	d1 [2] = -glm::dot ( n [2], loc );
	d2 [2] = -glm::dot ( n [2], loc + e1 );

	for ( int i = 0; i < 3; i++ )
		if ( d1 [i] > d2 [i] )		// flip normals, so that d1 < d2
		{
			d1 [i] = -d1 [i];
			d2 [i] = -d2 [i];
			n  [i] = -n [i];
		}
}

glm::vec3	CsgBox::findNormal ( HitData& hit ) const
{
	float		minDist = 1e10;
	int			index   = 0;
	glm::vec3	normal;

	for ( int i = 0; i < 3; i++ )
	{
		float	d     = glm::dot ( hit.pos,  n [i] );
		float	dist1 = fabs ( d + d1 [i] );	// distances from point to
		float	dist2 = fabs ( d + d2 [i] );	// pair of parallel planes

		if ( dist1 < minDist )
		{
			minDist = dist1;
			index   = i;
		}
		
		if ( dist2 < minDist )
		{
			minDist = dist2;
			index   = i;
		}
	}

	normal = n [index];			// normal to plane, the point belongs to
	
	if ( glm::dot ( hit.pos - center, normal ) < 0.0 )
		normal = -normal;		// normal must point outside of center

    return normal;
}

		// get all intersections, returns false if no intersections
bool CsgBox :: allIntersections ( const ray& r, std::vector<RaySegment>& res ) const
{
	float	tNear = -1e10;		// tNear = max t1
	float	tFar  =  1e10;		// tFar  = min t2
	float	t1, t2;

	for ( int i = 0; i < 3; i++ )		// process each slab
	{
		float	vd = glm::dot ( r.getDir    (), n [i] );
		float	vo = glm::dot ( r.getOrigin (), n [i] );

		if ( vd > EPS )			// t1 < t2, since d1 [i] < d2 [i]
		{
			t1 = -( vo + d2 [i] ) / vd;
			t2 = -( vo + d1 [i] ) / vd;
		}
		else
		if ( vd < -EPS )		// t1 < t2, since d1 [i] < d2 [i]
		{
			t1 = -( vo + d1 [i] ) / vd;
			t2 = -( vo + d2 [i] ) / vd;
		}
		else				// abs ( vd ) < Threshold => ray is parallel to slab
		{
			if ( vo < d1 [i] || vo > d2 [i] )
				return false;
			
			continue;
		}

		if ( t1 > tNear )
			tNear = t1;

		if ( t2 < tFar )
			if ( ( tFar = t2 ) < EPS )
				return false;

		if ( tNear > tFar )
			return false;
	}

	RaySegment	seg;
	
	seg.t      [0] = tNear;
	seg.t      [1] = tFar;
	seg.obj    [0] = this;
	seg.obj    [1] = this;
	seg.invert [0] = false;
	seg.invert [1] = false;
	seg.hit    [0].t = tNear;
	seg.hit    [1].t = tFar;
	seg.hit    [0].pos = r.pointAt ( tNear );
	seg.hit    [1].pos = r.pointAt ( tFar );
	seg.hit    [0].object = this;
	seg.hit    [1].object = this;
	seg.hit    [0].cache [0] = tNear;
	seg.hit    [0].cache [1] = tFar;
	seg.hit    [1].cache [0] = tNear;
	seg.hit    [1].cache [1] = tFar;

	res.push_back ( seg );

	return true;
}

		// get surface parameters at given point
void CsgBox :: getSurface ( HitData& hit, SurfaceData& surface ) const
{
	surface.pos      = hit.pos;
	surface.n        = findNormal ( hit );
	surface.color    = defSurface.color;
	surface.emission = defSurface.emission;
	surface.ka       = defSurface.ka;
	surface.kd       = defSurface.kd;
	surface.ks       = defSurface.ks;
	surface.kr       = defSurface.kr;
	surface.kt       = defSurface.kt;
	
}

////////////////////////////////////////////////////////

void RaySegment::intersect ( const RaySegment& s )
{
	if ( s.start () > start () )
		copyPoint ( s, 0, 0, false );
	
	if ( s.end () < end () )
		copyPoint ( s, 1, 1, false );
}

void RaySegment:: merge ( const RaySegment& s )
{
	if ( s.start () < start () )
		copyPoint ( s, 0, 0, false );
	
	if ( s.end () > end () )
		copyPoint ( s, 1, 1, false );
}

////////////////////////////////////////////////////////

void csgAnd ( const std::vector<RaySegment>& a, const std::vector<RaySegment>& b, std::vector<RaySegment>& c )
{
	auto ia = a.begin ();
	auto ib = b.begin ();
	
	c.clear ();
	
	if ( a.empty() || b.empty() )
		return;

	for ( ; ; )
	{
						// skip all a segments left of ib
		if ( ib != b.end () )
			while ( ia != a.end () && ia->end () < ib->start () )
				ia++;
						// skip all b segments to the left of ia
		if ( ia != a.end () )
			while ( ib != b.end () && ib->end () < ia->start () )
				ib++;
		
		if ( ia == a.end () || ib == b.end () )
			return;
		
		assert ( ia -> intersects ( *ib ) );
		
		RaySegment s = *ia++;
		
						// intersect s with b segments
		while ( ia != b.end () && s.intersects ( *ib ) )
		{
			RaySegment s1 = s;
			
			s1.intersect ( *ib++ );
			
			if ( !s1.empty () )
				c.push_back  ( s1 );
		}
	}
}

void csgSub ( const std::vector<RaySegment>& a, const std::vector<RaySegment>& b, std::vector<RaySegment>& c )
{
	auto ia = a.begin ();
	auto ib = b.begin ();
	
	c.clear ();
	
	if ( a.empty () )		// return empty set
		return;

	if ( b.empty () )
	{
		c = a;
		return;
	}

	for ( ; ; )
	{
		if ( ib != b.end () )
			while ((ia != a.end ()) && (ia->end () < ib->start () ) )
				ia++;
		
		if ( ia != a.end () )
			while ( (ib != b.end ()) && (ib->end () < ia->start () ) )
				ib++;
		
		if ( ia == a.end () )
			return;
		
		if ( ib == b.end () )
		{
			while ( ia != a.end () )
				c.push_back ( *ia++ );
			
			return;
		}
		
		assert ( ia->intersects(*ib) );
		
		RaySegment s = *ia++;
		
							// subtract b segments from s
		while ( (ib != b.end ()) && s.intersects ( *ib ) && !s.empty () )
		{
			RaySegment s1 = s;
			

			s1.copyPoint ( *ib, 0, 1, true );	// s1.end = ib->start
			s.copyPoint  ( *ib, 1, 0, true ); 	// s.start = ib->end;
			
			if ( !s1.empty () )
				c.push_back  ( s1 );
			
			ib++;
		}
		
		if ( !s.empty () )
			c.push_back ( s );
	}
}

void csgOr ( const std::vector<RaySegment>& a, const std::vector<RaySegment>& b, std::vector<RaySegment>& c )
{
	auto ia = a.begin ();
	auto ib = b.begin ();
	
	c.clear();

	if ( a.empty () )
	{
		c = b;
		return;
	}

	if ( b.empty () )
	{
		c = a;
		return;
	}

	for ( ; ; )
	{
		if ( ib != b.end () )
			while ((ia != a.end ()) && (ia->end () < ib->start () ) )
				ia++;
		
		if ( ia != a.end () )
			while ( (ib != b.end ()) && (ib->end () < ia->start () ) )
				ib++;
		
		if ( ia == a.end () ) 
			while ( ib != b.end () )
				c.push_back ( *ib++ );
			
		if ( ib == b.end () )
			while ( ia != a.end () )
				c.push_back ( *ia++ );
			
		if (( ia == a.end () ) && ( ib == b.end () ) )
			return;
		
		assert ( ia->intersects ( *ib ) );
		
		RaySegment s = *ia++;
		
		while (((ia != a.end ()) && s.intersects ( *ia)) || ((ib != b.end ()) && s.intersects (*ib)))
		{
			while ( ia != a.end () && s.intersects ( *ia ) )
				s.merge ( *ia++ );
			
			while ( ib != b.end () && s.intersects ( *ib ) )
				s.merge ( *ib++ );
			
		}
		
		c.push_back ( s );
	}
}
