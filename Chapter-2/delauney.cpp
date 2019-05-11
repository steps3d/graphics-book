inline operator < ( const glm::vec2& a, const glm::vec2& b )
{
	if ( a.x < b.x )
		return true;
	
	if ( a.x > b.x )
		return false;
	
	return a.y < b.y;
}

class edge
{
public:
	glm::vec2 first, second;
	
	edge ( const glm::vec2& a, const glm::vec2& b ) : first ( a ), second ( b ) {}
	
	operator < ( const edge& e ) const
	{
		if ( first < e.first )
			return true;
		
		if ( first > e.first )
			return false;
		
		if ( second < e.second )
			return true;
		
		return false;
	}
};

std::list<triangle> triangulate ( const glm::vec2 * p, int n )
{
		glm::vec2	          n, v;
		std::list<triangle> * result = new std::list<triangle>;
		std::set<edge>        frontier;
		
						// pick some edge
		edge e = hullEdge ( p, n );
		
		frontier.insert ( e );
		
		while ( !frontier.empty () )
		{
			e = removeMin ( frontier );
			
			if ( mate ( e, p, n, v ) )
			{
				updateFrontier ( frontier, v, e.first );
				updateFrontier ( frontier, e.second, v );
				result -> push_front ( triangle ( e.first, e.second, v ) );
		}
		
		return result;
}

void updateFrontier ( std::set<edge>& frontier, const glm::vec2& a, const glm::vec2& b )
{
	edge e = edge ( a, b );
	
	if ( std::hash_set<edge>::iterator it = frontier.find ( e ) != frontier.end () )
		frontier.erase ( it );
	else
		frontier.insert ( edge ( b, a ) );
}

edge hullEdge ( glm::vec2 * p, int n )
{
	int m = 0, i = 1;
	
	for ( ; i < n; i++ )
		if ( p[i].x < p[m].x || ((p[i].x == p [m].x) && (p[i].y < p[m].y)))
			m = i;
		
	std::swap ( p [0], p [m] );
	
	for ( m = 1, i = 2; i < n; i++ )
	{
		int c = classify ( p [0], p [m], p [i] );
		
		if ((c == LEFT) || (c == BETWEEN)))
			m = i;
	}
	
	return edge ( p [0], p [m] );
}

bool mate ( const edge& e, const glm::vec2 * p, int n, glm::vec2& v )
{
	glm::vec2 * bestVertex = nullptr;
	float		bestT      = FLT_MAX;
	edge        f = rotate ( e );
	
	for ( int i = 0; i < n; i++ )
		if ( classify ( e.first, e.second, p [p] ) == RIGHT )
		{
			edge g = rotate ( edge ( e.second, p [i] ) );
			
			intersect ( f, g, t );
			
			if ( t < bestT )
			{
				bestVertex = &p [i];
				bestT      = t;
			}
		}
		
	if ( bestVertex )
	{
		v = *bestVertex;
		
		return true;
	}
	
	return false;
}

int intersect ( const edge& f, const edge& e, float& t )
{
	glm::vec2	a = f.first;
	glm::vec2	b = f.second;
	glm::vec2	c = e.first;
	glm::vec2	d = e.second;
	glm::vec2	n ( d.y - c.y, c.x - d.x );	
	float		denom = glm::dot ( n, b - a );
	
	if ( fabs ( denom ) < EPS )
	{
		int cls = classify ( e.first, e.second, f.first );
		
		t = FLT_MAX;
		
		if ((cls == LEFT) || (cls == RIGHT))
			return PARALLEL;
			
		return COLLINEAR;
	}
	
	t = - glm::dot ( n, a -c ) / denom;
	
	return SKEW;
}

edge rotate ( const edge& e )
{
	glm::vec2	m = 0.5f * ( e.first + e.second );
	glm::vec2	v = e.second - e.first;
	glm::vec2	n ( v.y, -v.x );
	
	return edge ( m - 0.5f * n, m + 0.5f * n );
}
