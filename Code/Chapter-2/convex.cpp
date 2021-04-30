bool isConvex ( const glm::vec2 v [], int n )
{
    glm::vec2 prev = v [n-1];
    glm::vec2 cur, dir, nv;
    float     d, dp;
    int       sign, s;

    for ( int i = 0; i < n; i++ )
    {
        cur  = v [i];
        dir  = cur - prev;                 // edge direction vector
        n    = glm::vec2 ( dir.y, -dir.x );// normal to edge [prev, cur]
        d    = -glm::dot ( nv, cur );    // so line's equation is (p,n) + d = 0
        sign = 0;                          // unknown yet

        for ( int j = 0; j < n; j++ )
        {
            dp = d + glm::dot ( nv, v [j] );

            if ( fabs ( dp ) < EPS )       // too small
               continue;

            s = dp > 0 ? 1 : -1;           // sign for v [j]

            if ( sign == 0 )
               sign = s;
            else
            if ( sign != s )
                return false;
        }

        prev = cur;
    }

    return true;
}
