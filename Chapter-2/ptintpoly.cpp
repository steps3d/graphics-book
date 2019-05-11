bool ptInPoly( const glm::vec2 p [], int n, const glm::vec2& pt )
{
    int i, j, c = 0;
	
    for ( i = 0, j = n - 1; i < n; j = i++ )
    {
        if ((((p.y [i] <= pt.y) && (pt.y < p.y [j])) || 
            ((p.y [j] <= pt.y) && (pt.y < p.y [i]))) && 
            (pt.x < (p.x [j]-p.x [i])*(pt.y-p.y [i])/
            (pt.p.y [j]-p.y [i])+p.x [i]))
               c = !c;
    }
	
    return c != 0;
}
