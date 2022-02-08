bool doLinesCoincide ( const glm::vec2& n1, float d1,
                       const glm::vec2& n2, float d2 )
{
    n1 = glm::normalize(n1);
    n2 = glm::normalize(n2);

    float ncos = glm::dot(n1 , n2);

    // Angle between normals is 0 degrees (cos = 1)
	if (fabs(ncos - 1) < EPS)
    {
		return fabs ( d1 - d2 ) < EPS;
    }

    // Angle between normals is 180 degrees (cos = -1)
	else if (fabs(ncos + 1) < EPS)
    {
		return fabs ( d1 + d2 ) < EPS;
    }

    return false;
}

bool findLineIntersection ( const glm::vec2& n1, float d1,
                            const glm::vec2& n2, float d2,
                            glm::vec2& p )
{
	const float     det = n1.x * n2.y - n1.y * n2.x;

	if ( fabs ( det ) < EPS )
		return doLinesCoincide ( n1, d1, n2, d2 );

	p.x = (d2*n1.y - d1*n2.y)/det;
	p.y = (d1*n2.x - d2*n1.x)/det;

	return true;
}

bool findSegIntersection ( const glm::vec2& a1, const glm::vec2& b1,
                           const glm::vec2& a2, const glm::vec2& b2,
                           glm::vec2& p )
{
	const glm::vec2 l1 = b1 - a1;
	const glm::vec2 l2 = b2 - a2;
	const glm::vec2 n1 ( l1.y , -l1.x );
	const glm::vec2 n2 ( l2.y , -l2.x );
	const float     d1 = -glm::dot ( a1, n1 );
	const float     d2 = -glm::dot ( a2, n2 );

		// use determinants to check for coinsiding
		// since vectors are normalized
	const float t1 = n1.x*n2.y - n1.y*n2.x;
	const float t2 = n1.x*d2   - n2.x*d1;

		// check for degenerate case
	if ( fabs ( t1 ) < EPS && fabs ( t2 ) < EPS )
	{
		if ( fabs ( a1.x ) > EPS ) // project on Ox
		{
			if ( max ( a1.x , a2.x ) > min ( b1.x , b2.x ) )
				return false;

				// pick a1 or a2 as start of intersection
			p = ( a1.x > a2.x ? a1 : a2 );

			return true;
		}
		else
		if ( fabs ( a1.y ) > EPS ) // project on Oy
		{
			if ( max ( a1.y , a2.y ) > min ( b1.y , b2.y ) )
				return false;

			p = ( a1.y > a2.y ? a1 : a2 );

			return true;
		}

		return false;	// incorrect data
	}
		// find lines intersection
	const float     det = n1.x * n2.y - n1.y * n2.x;
	const glm::vec2 p0 ( d2*n1.y - d1*n2.y, d1*n2.x - d2*n1.x )/det;

	bool  res = min( a1.x, b1.x ) <= p0.x &&
	       p0.x <= max( a1.x, b1.x ) &&
	       min( a1.y, b1.y ) <= p0.y &&
	       p0.y <= max( a1.y, b1.y );

	if ( res )
		p = p0;

	return res;
}
