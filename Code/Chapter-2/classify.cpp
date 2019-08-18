int classify ( const glm::vec2& a, const glm::vec2& b, const glm::vec2& p )
{
	const glm::vec2 a2b  = b - a;	// от a к b
	const glm::vec2 a2p  = p - a;	// от a к p

				// проверить на совпадение концов отрезка
	if ( fabs ( a2b.x ) + fabs ( a2b.y ) < EPS )
		if ( fabs ( a2p.x ) + fabs ( a2p.y ) < EPS )
			return DEGENERATE_COINCIDE;	// a == b == p
		else
			return DEGENERATE_DIFFERENT;	// a == b != p
			                             
	float	area = a2b.x*a2p.y - a2p.x*a2b.y;

	if ( area > EPS )
		return LEFT;

	if ( area < -EPS )
		return RIGHT;

	if ( glm::length ( p - a ) < EPS )
		return START;

	if ( glm::length ( p - b ) < EPS )
		return END;

	if ((a2b.x*a2p.x < 0) || (a2b.y*a2p.y < 0))
		return BEHIND;

	if ( glm::length(a2b) < glm::length(a2p) )
		return BEYOND;

	return BETWEEN;
}

int classifyLineCircle ( const glm::vec2& n, float d, 
                         const glm::vec2& c, float r )
{
	const float signedDistance = glm::dot ( c, n ) + d;

	if ( signedDistance >= r + EPS )
		return IN_POSITIVE;

	if ( signedDistance <= -r - EPS )
		return IN_NEGATIVE;

	return IN_BOTH;
}

int classifyBox ( const glm::vec2& pMin, const glm::vec2& pMax, const glm::vec2& n, float d )
{
	glm::vec2 pn ( n.x >= 0 ? pMin.x : pMax.x, n.y >= 0 ? pMin.y : pMax.y );
	const float     f = glm::dot ( n, pn ) + d;
	
	if ( f > 0 )
		return IN_POSITIVE;
	
	pn.x = n.x >= 0 ? pMax.x : pMin.x;
	pn.y = n.y >= 0 ? pMax.y : pMin.y;
	
	if ( glm::dot ( n, pn ) + d < 0 )
		return IN_BEGATIVE;
	
	return IN_BOTH;
}
