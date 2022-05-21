//
// Class for simple plane (p,n) + dist = 0
//
// Author: Alexey V. Boreskov
//

#pragma once
#ifndef __PLANE__
#define __PLANE__

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>

#ifndef	EPS
#define	EPS 0.00001f
#endif

enum
{
	IN_FRONT = 1,
	IN_BACK  = 2,
	IN_BOTH  = 3,
	IN_PLANE = 4
};

class   plane
{
public:
	glm::vec3 	n;          		// normal vector
	float   	dist = 0;       	// signed distance along n
						// build plane from normal and signed distance
	unsigned      	nearPointMask = 0;	//  if not initialized
	unsigned      	mainAxis      = 0;	// index of main axis

	plane () : n ( 0, 0, 1 )
	{
		computeNearPointMaskAndMainAxis ();
	}

	plane ( const glm::vec3& normal, float d ) : n ( normal ), dist ( d )
	{
		float	len = glm::length ( n );

		n /= len;
		//d /= len;

		computeNearPointMaskAndMainAxis ();
	}
                                // build plane from plane equation
	plane ( float nx, float ny, float nz, float d ) : n (nx, ny, nz), dist ( d )
	{
		float	len = glm::length ( n );

		n /= len;
		d /= len;

		computeNearPointMaskAndMainAxis ();
	}
                                // build plane from normal and point on plane
	plane ( const glm::vec3& normal, const glm::vec3& point ) : n ( normal )
	{
		n    = glm::normalize ( n );
		dist = - glm::dot( point, n );

		computeNearPointMaskAndMainAxis ();
	}
                                // build plane from 3 points
	plane ( const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3 )
	{
		n = glm::cross ( p2 - p1, p3 - p1 );
		n = glm::normalize ( n );
		dist = -glm::dot ( p1, n );

		computeNearPointMaskAndMainAxis ();
	}

	plane ( const plane& plane ) : n ( plane.n ), dist ( plane.dist )
	{
		nearPointMask = plane.nearPointMask;
		mainAxis      = plane.mainAxis;
	}

	void     setFromEquation ( float a, float b, float c, float d )
	{
		n    = glm::vec3 ( a, b, b );
		dist =  -d;

		float	len = glm::length ( n );

		n  /= len;
		dist /= len;

		computeNearPointMaskAndMainAxis ();
	}
	
	float    signedDistanceTo ( const glm::vec3& v ) const
	{
		return glm::dot ( v, n ) + dist;
	}

	float	distanceTo ( const glm::vec3& v ) const
	{
		return fabs ( signedDistanceTo ( v ) );
	}
	                            // get point on plane
	glm::vec3    point () const
	{
		return (-dist) * n;
	}
                            // classify point
	int	classify ( const glm::vec3& p ) const
	{
		float   v = signedDistanceTo ( p );

		if ( v > EPS )
			return IN_FRONT;
        
		if ( v < -EPS )
			return IN_BACK;

		return IN_PLANE;
	}

    void    flip ()
    {
        n    = -n;
        dist = -dist;

        computeNearPointMaskAndMainAxis ();
    }

    float   closestPoint ( const glm::vec3& p, glm::vec3& res ) const
    {
        float   distanceToPlane = - dist - glm::dot ( p, n );

        res = p + distanceToPlane * n;

        return distanceToPlane;
    }

    bool	intersectByRay ( const glm::vec3& org, const glm::vec3& dir, float& t ) const
    {
        float   numer = - (dist + glm::dot ( org, n ));
        float   denom = glm::dot ( dir, n );

        if ( fabs ( denom ) < EPS )
           return false;

		t = numer / denom;

        return true;
    }

                            // mirror position (point), depends on plane posit.
    void    reflectPos ( glm::vec3& v ) const
    {
        v -= (2.0f * (glm::dot ( v, n ) + dist)) * n;
    }

                            // mirror direction, depends only on plane normal
    void    reflectDir ( glm::vec3& v ) const
    {
        v -= (2.0f * glm::dot ( v, n )) * n;
    }

    glm::vec3    makeNearPoint ( const glm::vec3& minPoint, const glm::vec3& maxPoint ) const
    {
        return glm::vec3 ( nearPointMask & 1 ? maxPoint.x : minPoint.x,
                           nearPointMask & 2 ? maxPoint.y : minPoint.y,
                           nearPointMask & 4 ? maxPoint.z : minPoint.z );
    }

    glm::vec3    makeFarPoint ( const glm::vec3& minPoint, const glm::vec3& maxPoint ) const
    {
        return glm::vec3 ( nearPointMask & 1 ? minPoint.x : maxPoint.x,
                           nearPointMask & 2 ? minPoint.y : maxPoint.y,
                           nearPointMask & 4 ? minPoint.z : maxPoint.z );
    }

    int getMainAxis () const;

protected:
    void    computeNearPointMaskAndMainAxis ();
};

void	extractClipPlace ( plane p [6], const glm::mat4& m );

#endif
