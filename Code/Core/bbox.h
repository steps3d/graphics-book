//
// Axis-aligned bounding box
//
// Author: Alexey V. Boreskov
//

#pragma once
#ifndef __BOUNDING_BOX__
#define __BOUNDING_BOX__

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>			// for make_mat
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <limits>
#include "plane.h"

class   bbox
{
	glm::vec3    minPoint;
	glm::vec3    maxPoint;

public:
	bbox ()
	{
		reset ();
	}

	bbox ( const glm::vec3& v1, const glm::vec3& v2 );
	bbox ( const bbox& b1, const bbox& b2 ) : minPoint ( b1.minPoint ), maxPoint ( b1.maxPoint )
	{
		merge ( b2 );
	}

	const glm::vec3&	getMinPoint () const
	{
		return minPoint;
	}

	const glm::vec3&	getMaxPoint () const
	{
		return maxPoint;
	}

	glm::vec3    getVertex ( int index ) const
	{
		return glm::vec3 ( index & 1 ? maxPoint.x : minPoint.x,
                           index & 2 ? maxPoint.y : minPoint.y,
                           index & 4 ? maxPoint.z : minPoint.z );
	}

	glm::vec3	getCenter () const
	{
		return (minPoint + maxPoint) * 0.5f;
	}

	glm::vec3	getSize () const
	{
		return glm::vec3 ( maxPoint.x - minPoint.x, maxPoint.y - minPoint.y, maxPoint.z - minPoint.z );
	}

	bbox&   addVertex   ( const glm::vec3& v );
	bbox&	addVertices ( const glm::vec3 * v, int numVertices );
	
   	int 	classify    ( const plane& plane ) const;
	
	bool	isEmpty () const
	{
		return minPoint.x > maxPoint.x || minPoint.y > maxPoint.y || minPoint.z > maxPoint.z;
	}

	bool	contains ( const glm::vec3& pos ) const
	{
		return pos.x >= minPoint.x && pos.x <= maxPoint.x &&
			pos.y >= minPoint.y && pos.y <= maxPoint.y &&
			pos.z >= minPoint.z && pos.z <= maxPoint.z;
	}

	bool	intersects ( const bbox& box ) const
	{
		if (( maxPoint.x < box.minPoint.x) || (minPoint.x > box.maxPoint.x) )
			return false;

		if (( maxPoint.y < box.minPoint.y) || (minPoint.y > box.maxPoint.y) )
			return false;

		if (( maxPoint.z < box.minPoint.z) || (minPoint.z > box.maxPoint.z) )
			return false;

		return true;
	}

		// spceify empty bbox: min=inf, max=-inf
	void    reset ()
	{
		minPoint.x =  std::numeric_limits<float>::max();
		minPoint.y =  std::numeric_limits<float>::max();
		minPoint.z =  std::numeric_limits<float>::max();
		maxPoint.x = -std::numeric_limits<float>::max();
		maxPoint.y = -std::numeric_limits<float>::max();
		maxPoint.z = -std::numeric_limits<float>::max();
	}

	void merge ( const bbox& box )
	{
		if ( box.minPoint.x < minPoint.x )
			minPoint.x = box.minPoint.x;

        if ( box.minPoint.y < minPoint.y )
           minPoint.y = box.minPoint.y;

        if ( box.minPoint.z < minPoint.z )
           minPoint.z = box.minPoint.z;

        if ( box.maxPoint.x > maxPoint.x )
           maxPoint.x = box.maxPoint.x;

        if ( box.maxPoint.y > maxPoint.y )
           maxPoint.y = box.maxPoint.y;

        if ( box.maxPoint.z > maxPoint.z )
           maxPoint.z = box.maxPoint.z;
	}

	void	grow ( const glm::vec3& delta )
	{
		minPoint -= delta;
		maxPoint += delta;
	}

	void	grow ( float delta )
	{
		minPoint.x -= delta;
		minPoint.y -= delta;
		minPoint.z -= delta;
		maxPoint.x -= delta;
		maxPoint.y -= delta;
		maxPoint.z -= delta;
	}
	
	void intersect ( const bbox& other )
	{
		minPoint = glm::max ( minPoint, other.minPoint );
		maxPoint = glm::min ( maxPoint, other.maxPoint );
	}

	float volume () const
	{
		glm::vec3	size = maxPoint - minPoint;

		return size.x * size.y * size.z;
	}

	float area () const
	{
		glm::vec3	size = maxPoint - minPoint;

		return (size.x*size.y + size.x*size.z + size.y*size.z) * 2;
	}

	void	apply ( const glm::mat4& m )
	{
		if ( isEmpty () )		// leave it empty
			return;

		auto	mn = m * glm::vec4 ( minPoint, 1 );
		auto	mx = m * glm::vec4 ( maxPoint, 1 );
		auto	minNew = glm::vec3 ( mn.x, mn.y, mn.z ) / mn.w;
		auto	maxNew = glm::vec3 ( mx.x, mx.y, mx.z ) / mx.w;

		minPoint = glm::min ( minNew, maxNew );
		maxPoint = glm::max ( minNew, maxNew );
	}
/*
									// distance from point along given direction to this box
	float	getDistanceTo ( const vec3& from, const vec3& dir ) const
	{
		return (getVertex ( computeNearPointMask ( dir ) ) - from) & dir;
	}

									// estimate distance from point given near point mask and main axis of direction
	float	getDistanceToAlongAxis ( const vec3& from, int index, int axis ) const
	{
		return (float)fabs ( (getVertex ( index ) - from) [axis] );
	}
*/

	float	distanceTo ( const plane& plane ) const
	{
		float	dist = plane.signedDistanceTo ( plane.makeNearPoint ( minPoint, maxPoint ) );

		if ( dist > 0 )						// box lies in positive halfspace, return positive distance
			return dist;

		dist = plane.signedDistanceTo ( plane.makeFarPoint ( minPoint, maxPoint ) );

		if ( dist < 0 )						// box lies in negative halfspace
			return dist;

		return 0;						// plane crosses this box
	}

	float	distanceTo ( const bbox& box ) const
	{
		float	distSqr = 0;

		for ( int i = 0; i < 3; i++ )
		{
			float	d1 = maxPoint [i] - box.minPoint [i];
			float	d2 = minPoint [i] - box.maxPoint [i];

			if ( d1 > 0 )
				distSqr += d1*d1;

			if ( d2 > 0 )
				distSqr += d2*d2;
		}

		return sqrtf ( distSqr );
	}
};

#endif
