bool pointInTriangle ( const glm::vec2& a, const glm::vec2& b, const glm::vec2& c,
                       glm::vec2& p )
{
	const glm::vec2 a1 = a - c;
	const glm::vec2 b1 = b - c;
	const float     aa = glm::dot(a1, a1);
	const float	ab = glm::dot(a1, b1);
	const float     bb = glm::dot(b1, b1);
	const float	d  = aa*bb - ab*ab;

	if ( fabs ( d ) < EPS )
		return false;
	
	const glm::vec2 p1 = p - c;
	const float     pa = glm::dot(p1, a1);
	const float	pb = glm::dot(p1, b1);
	const float     u  = (pa*bb - pb*ab) / d;

	if ( u < 0 || u > 1 )
		return false;

	const float     v = (pa*ab - pb*aa) / d;

	if ( v < 0 || v > 1 )
		return false;

	return u + v <= 1;
}
