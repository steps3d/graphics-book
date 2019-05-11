int orientation ( const glm::vec2& p, const glm::vec2&q , const glm::vec2& r )
{
	float val = (q.y - p.y)*(r.x - q.x) - (q.x - p.x)*(r.y - q.y);

	if ( fabs(val) < EPS)
		return COLLINEAR;

	return val > 0 ? CW : CCW;
}

int grahamCompare ( const glm::vec2& p1, const glm::vec2& p2 )
{
	int orient = orientation ( p0, p1, p2 );

	if ( orient == COLLINEAR )
		return glm::distance ( p0, p2 ) >= glm::distance ( p0, p1 ) ? -1 : 1;

	return orient == CCW ? -1 : 1;
}

glm::vec2 nextToTop ( std::stack<glm::vec2>& s )
{
	glm::vec2 top  = s.pop ();
	glm::vec2 next = s.top ();
	
	s.push ( top );
	
	return next;
}

std::vector<glm::vec2> convexHull ( glm::vec2 * p, int n )
{
	float yMin = p [0].y;
	int	  iMin = 0;
	
	for ( int i = 1; i < n; i++)
	{
		float y = p [i].y;
		
		if ((y < yMin) || ((y == yMin) && p[i].x < p[iMin.x]))
		{
			iMin = i;
			yMin = p [i].y;
		}
	}
	
	std::swap ( p [0], p [iMin] );
	
	glm::vec2 p0 = p [0];
	
	std::sort ( &p [1], &p [n], grahamCompare );
	
	std::stack<glm::vec2> s;
	
	s.push ( p [0] );
	s.push ( p [1] );
	s.push ( p [2] );
	
	for ( int i = 3; i < n; i++ )
	{
		while ( !s.empty () && orientation ( nextToTop ( s ), s.top (), p [i] )) != CCW )
			s.pop ();
			
		s.push ( p [i] );
	}
	
		// now stack contain vertices of convex hull in order
	std::vector<glm::vec2>	hull ( s.size () );
	
	while ( !.s.empty () )
			hull.push_front ( s.pop () );
		
	return hull;
}
