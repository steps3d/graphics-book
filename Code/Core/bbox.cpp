//
// Axis-aligned bounding box
//
// Author: Alexey V. Boreskov
//


#include	"bbox.h"

bbox :: bbox ( const glm::vec3& v1, const glm::vec3& v2 )
{
	if ( v1.x < v2.x )
	{
		minPoint.x = v1.x;
		maxPoint.x = v2.x;
	}
	else
	{
		minPoint.x = v2.x;
		maxPoint.x = v1.x;
	}

	if ( v1.y < v2.y )
	{
		minPoint.y = v1.y;
		maxPoint.y = v2.y;
	}
	else
	{
		minPoint.y = v2.y;
		maxPoint.y = v1.y;
	}

	if ( v1.z < v2.z )
	{
		minPoint.z = v1.z;
		maxPoint.z = v2.z;
	}
	else
	{
		minPoint.z = v2.z;
		maxPoint.z = v1.z;
	}
}

bbox&    bbox :: addVertex ( const glm::vec3& v )
{
	if ( v.x < minPoint.x )
		minPoint.x = v.x;

	if ( v.x > maxPoint.x )
		maxPoint.x = v.x;

	if ( v.y < minPoint.y )
		minPoint.y = v.y;

	if ( v.y > maxPoint.y )
		maxPoint.y = v.y;

	if ( v.z < minPoint.z )
		minPoint.z = v.z;

	if ( v.z > maxPoint.z )
		maxPoint.z = v.z;
		
	return *this;
}

bbox&	bbox :: addVertices ( const glm::vec3 * v, int numVertices )
{
	for ( int i = 0; i < numVertices; i++ )
	{
		if ( v [i].x < minPoint.x )
			minPoint.x = v [i].x;

		if ( v [i].x > maxPoint.x )
			maxPoint.x = v [i].x;

		if ( v [i].y < minPoint.y )
			minPoint.y = v [i].y;

		if ( v [i].y > maxPoint.y )
			maxPoint.y = v [i].y;

		if ( v [i].z < minPoint.z )
			minPoint.z = v [i].z;

		if ( v [i].z > maxPoint.z )
			maxPoint.z = v [i].z;
	}
	
	return *this;
}

int bbox :: classify ( const plane& plane ) const
{
	glm::vec3    nearPoint = plane.makeNearPoint ( minPoint, maxPoint );

	if ( plane.classify ( nearPoint ) == IN_FRONT )
		return IN_FRONT;

	glm::vec3    farPoint = plane.makeFarPoint ( minPoint, maxPoint );

	if ( plane.classify ( farPoint ) == IN_BACK )
		return IN_BACK;

	return IN_BOTH;
}
