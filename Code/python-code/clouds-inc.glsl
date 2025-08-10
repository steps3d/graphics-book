//
// Basic clouds definitions
//

const float noiseSpaceScale = 0.5;
const float fbmScale        = 1.3;
const vec3  timeScale       = 0.03 * vec3 ( 1.0, -0.2, -1.0 );
const float texSize         = 32.0;
const float PI              = 3.1415926;

float BeersLaw ( float dist, float absorption )
{
    return exp ( -dist * absorption );
}

float    pow15 ( float x )
{
    return x * sqrt ( x );
}

float    HenyeyGreenstein ( float g, float mu )
{
    float    g2 = g * g;
    
    return (1.0 / (4.0 * PI)) * ( (1.0 - g2) / pow15 ( 1.0 + g2 - 2.0 * g * mu ) );
}

float sdSphere ( vec3 p, vec3 center, float radius )
{
    return length ( p - center ) - radius;
}

float noise3D ( vec3 p )
{
    return textureLod ( noiseMap, p, 0 ).r * 2.0 - 1.0;
}

float fBm ( vec3 p )
{
    vec3     q      = p + time * timeScale;
    float    f      = 0.0;
    float    scale  = 0.51;
    float    factor = 2.02;

    for ( int i = 0; i < 4; i++ )
    {
        f      += scale * noise3D ( q );
        q      *= factor;
        factor += 0.21;
        scale  *= 0.5;
    }

    return f;
}

float fBm ( vec3 p, bool lowRes )
{
    vec3    q          = p + time * 0.03 * vec3 ( 1.0, -0.2, -1.0 );
    float   f          = 0.0;
    float   scale      = 0.51;
    float   factor     = 2.02;
    int     numOctaves = lowRes ? 4 : 3;

    for ( int i = 0; i < numOctaves; i++ )
    {
        f      += scale * noise3D ( q );
        q      *= factor;
        factor += 0.21;
        scale  *= 0.5;
    }

    return f;
}

float scene ( vec3 p, bool lowRes )
{
    p *= mv;
    
    float    d1       = sdSphere ( p, vec3 ( 0, -1,   0.3 ), 1.3 );
    float    d2       = sdSphere ( p, vec3 ( 0,  1.2, 0 ),   1.5 );
    float    distance = min      ( d1, d2 );
    float    f        = fBm      ( noiseSpaceScale * p, lowRes );

    return -distance + fbmScale * f;
}
