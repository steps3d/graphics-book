bool ptInPoly( const glm::vec2 p [], int n, const glm::vec2& pt )
{
    int i, j, c = 0;

    for ( i = 0, j = n - 1; i < n; j = i++ )
    {
        bool ptYWithinBounds = (
            ((p[i].y <= pt.y) && (pt.y < p[j].y)) ||
            ((p[j].y <= pt.y) && (pt.y < p[i].y))
        );

        bool ptXWithinBounds = (
            pt.x < (p[j].x - p[i].x) * (pt.y - p[i].y) / (p[j].y - p[i].y) + p[i].x
        );

        if (ptYWithinBounds && ptXWithinBounds)
        {
            c = !c;
        }
    }

    return c != 0;
}
