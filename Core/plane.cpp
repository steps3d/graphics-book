//
// Class for simple plane (p,n) + dist = 0
//
// Author: Alex V. Boreskoff
//

#include    "plane.h"

int	computeNearPointMask ( const glm::vec3& n )
{
    if ( n.x > 0.0f )
        if ( n.y > 0.0f )
            if ( n.z > 0.0f )
                return 0;
            else
                return 4;
        else
            if ( n.z > 0.0f )
                return 2;
            else
                return 6;
    else
        if ( n.y > 0.0f )
            if ( n.z > 0.0f )
                return 1;
            else
                return 5;
        else
            if ( n.z > 0.0f )
                return 3;
            else
                return 7;
}

void    plane :: computeNearPointMaskAndMainAxis ()
{
	nearPointMask = computeNearPointMask ( n );
	mainAxis      = getMainAxis ();			// XXX
}

int	plane::getMainAxis () const
{
	int	axis = 0;
	float	val  = fabs ( n.x );

	for ( int i = 1; i < 3; i++ )
	{
		float	vNew = fabs ( n [i] );

		if ( vNew > val )
		{
			val  = vNew;
			axis = i;
		}
	}

	return axis;
}

void	extractClipPlace ( plane p [6], const glm::mat4& m )
{
		// left 
	p [0].setFromEquation ( m [3][0] + m [0][0], m [3][1] + m [0][1], m [3][2] + m [0][2], m [3][3] + m [0][3] );

		// right
	p [1].setFromEquation ( m [3][0] - m [0][0], m [3][1] - m [0][1], m [3][2] - m [0][2], m [3][3] - m [0][3] );

		// bottom
	p [2].setFromEquation ( m [3][0] + m [1][0], m [3][1] + m [1][1], m [3][2] + m [1][2], m [3][3] + m [1][3] );

		// up
	p [3].setFromEquation ( m [3][0] - m [1][0], m [3][1] - m [1][1], m [3][2] - m [1][2], m [3][3] - m [1][3] );

		// near
	p [4].setFromEquation ( m [3][0] + m [2][0], m [3][1] + m [2][1], m [3][2] + m [2][2], m [3][3] + m [2][3] );

		// far
	p [5].setFromEquation ( m [3][0] - m [2][0], m [3][1] - m [2][1], m [3][2] - m [2][2], m [3][3] - m [2][3] );
}

