float signedArea ( const glm::vec2 p [], int n )
{
    float sum = p [0].x   * (p [1].y - p [n-1].y) + 
                p [n-1].x * (p [0].y - p [n-2].y);
	 
    for ( int i = 1; i < n - 1; i++ )
        sum += p [i].x * (p [i+1].y - p [i-1].y);

    return 0.5f * sum;
}

bool isClockwise ( const glm::vec2 p [], int n )
{
	return signedArea ( p, n ) > 0;
}
