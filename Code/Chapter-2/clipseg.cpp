glm::vec2 outerNormal ( const glm::vec2 p [], int n , int i )
{
	const int i1 = (i+1) % n;
	const int j  = (i+2) % n;
	glm::vec2 n ( p[i].y - p[i1].y, p[i1].x - p[i].y );

	if ( glm::dot ( p[j] - p[i], n ) > 0 )
		n = -n;

	return n;
}

bool clipSegByConvexPoly ( const glm::vec2 p [], int n, 
                           glm::vec2& a, glm::vec2& b )
{
	const glm::vec2 d    = b - a;
	float 	        tIn  = -INFINITY;
	float	        tOut = INFINITY;
	
	for ( int j = n - 1, i = 0; i < n; i++ )
	{
		const glm::vec2 n  = outerNormal ( p, n, j );	// outer normal for p[j]p[i]
		const float	dn = glm::dot ( d, n );
		
		if ( fabs ( dn ) < EPS )	// parallel
		{
			if ( glm::dot ( p [j] - a, n ) > 0 )
				return false;
			
			continue;
		}
		
		const float	pn = glm::dot ( p[j] - a, n );
		const float	t  = pn / dn;
		
		if ( t > 0 )
			tOut = min ( tOut, t );
		else
			tIn = max ( tIn, t );
		
		if ( tOut <= tIn )
			return false;
	}
	
	tIn  = max ( tIn, 0 );
	tOut = min ( tOut, 1 );
	
	if ( tOut <= tIn )
		return false;
	
	b = a + tOut * d;
	a = a + tIn  * d;
	
	return true;
}

bool clipSegByRect ( const glm::vec2& rMin, const glm::vec2& rMax,
                     glm::vec2& a, glm::vec2& b )
{
	float		p [4], q [4];
	glm::vec2	d = b - a;
	float	t1 = 0, t2 = 0;
	 
	 
	p [0] = -d.x;
	p [1] = d.x;
	p [2] = -d.y;
	p [3] = d.y;
	q [0] = a.x - rMin.x;
	q [1] = rMax.x - a.x;
	q [2] = a.y - rMin.y;
	q [3] = rMax.y - a.y;
	 
	for ( int i = 0; i < 4; i++ )
	{
		if ( fabs ( p [i] ) < EPS )	// line is parallel
		{
			if ( q [i] >= 0 )
			{
				if ( i < 2 ) 
				{
					if ( a.y < rMin.y )
						a.y = rMin.y;
						
					if ( b.y > rMax.y )
						b.y = rMax.y;
				}
				 
				if ( i > 1 )
				{
					if ( a.x < rMin.x )
						a.x = rMin.x;
					 
					if ( b.x > rMax.x )
						b.x = rMax.x;
				}
			}
		}
	}
	 
	for ( int i = 0; i < 4; i++ )
	{
		if ( fabs ( p [i] ) < EPS )
			continue;
		
		float	temp = q [i] / p [i];
		 
		if ( p [i] < 0 )
		{
			if ( t1 <= temp )
				t1 = temp;
		}
		else
		{
			if ( t2 > temp ) 
				t2 = temp;
		}
	}
	
	
	if ( t1 >= t2 )
		return false;
	
	b = a + t2 * d;
	a = a + t1 * d;
	
	return true;
}
 