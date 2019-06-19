float squaredPointToSegDistance ( const glm::vec2& p, 
                  const glm::vec2& a, const glm::vec2& b, 
                  float& t )
{
	const glm::vec2 delta = b - a;
	const glm::vec2 diff  = p - delta;

	if ( (t = glm::dot ( a, diff ) > 0 )
	{
		float dt = glm::dot(delta, delta);

		if ( t < dt )
		{
			t    /= dt;
//			diff -= t * a;
		}
		else
		{
			t     = 1;
//			diff -= a;
		}
	}
	else
		t = 0;
	
	glm::vec2 p = a + t*delta - p;

	return glm::dot(p, p);
}
