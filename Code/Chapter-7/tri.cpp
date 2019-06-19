#include	<math.h>
#include	"TgaImage.h"

template <typename T>
T abs ( T x )
{
	return x > 0 ? x : -x;
}

struct point
{
	int x, y;
};

inline void buildPlane ( 
        const point& p1, const point& p2, 
        const point& p3,
        int& a, int& b, int&c )
{
		// find line equations from p1 and p2
	a = p2.y - p1.y;
	b = p1.x - p2.x;
	c = -a*p1.x - b*p1.y;

		// check whether p3 is in positive
	if ( a*p3.x+b*p3.y+c < 0 )
	{
		a = -a;
		b = -b;
		c = -c;
	}
}

inline static bool isEdgeLeft ( const point& p1, const point& p2 )
{
	return p1.y < p2.y;
}

inline static bool isEdgeTop ( const point& p1, const point& p2 )
{
	return (p1.y == p2.y) && (p1.x >= p2.x);
}

void rasterizeTriangle( TgaImage& image, point p [], uint32_t color )
{
	int xMin = p[0].x;
	int yMin = p[0].y;
	int xMax = p[0].x;
	int yMax = p[0].y;
	int a [3], b[3], c[3];

		// find bounding box
	for ( int i = 1; i < 3; i++ )
	{
		if ( p [i].x < xMin )
			xMin = p[i].x;
		else
		if ( p [i].x > xMax )
			xMax = p[i].x;

		if ( p [i].y < yMin )
			yMin = p[i].y;
		else
		if ( p [i].y > yMax )
			yMax = p[i].y;
	}

		// build line equations
	buildPlane ( p[0], p[1], p[2], a[0], b[0], c[0] );
	buildPlane ( p[0], p[2], p[1], a[1], b[1], c[1] );
	buildPlane ( p[1], p[2], p[0], a[2], b[2], c[2] );

	bool	edgeInside [3];
	
	edgeInside [0] = isEdgeTop ( p [0], p [1] ) || isEdgeLeft ( p [0], p [1] );
	edgeInside [1] = isEdgeTop ( p [2], p [0] ) || isEdgeLeft ( p [2], p [0] );
	edgeInside [2] = isEdgeTop ( p [1], p [2] ) || isEdgeLeft ( p [1], p [2] );
	
		// find functions at lower-left corner
	int d0 = a[0]*xMin + b[0]*yMin + c[0];
	int d1 = a[1]*xMin + b[1]*yMin + c[1];
	int d2 = a[2]*xMin + b[2]*yMin + c[2];
  
		// check points
	for ( int y = yMin; y <= yMax; y++ )
	{
		int f0 = d0, 
			f1 = d1,
			f2 = d2;

		d0 += b[0];
		d1 += b[1];
		d2 += b[2];

		for ( int x = xMin; x <= xMax; x++, f0 += a[0], f1 += a[1], f2 += a[2] )
		{
			if ( f0 < 0 || (f0 == 0 && !edgeInside [0]) )
				continue;
			
			if ( f1 < 0 || (f1 == 0 && !edgeInside [1]) )
				continue;

			if ( f2 < 0 || (f2 == 0 && !edgeInside [2]) )
				continue;

			image.putPixel( x, y, color );
		}
	}
}

void drawLine ( TgaImage& image, int xa, int ya, int xb, int yb, 
                uint32_t color )
{
	int dx = abs(xb - xa);
	int dy = abs(yb - ya);
	int sx = xb >= xa ? 1 : -1;    // sign of xb-xa
	int sy = yb >= ya ? 1 : -1;    // sign of yb-ya

	if ( dy <= dx )
	{
		int d  = (dy<<1) - dx;
		int d1 = dy << 1;
		int d2 = (dy - dx) << 1;
		int x  = xa + sx;
		int y  = ya;

		image.putPixel ( xa, ya, color );
		for ( int i = 1; i <= dx; i++, x += sx )
		{
			if ( d > 0 )
			{
				d += d2;
				y += sy;
			}
			else
				d += d1;

			image.putPixel ( x, y, color );
		}
	}
	else
	{
		int d  = (dx<<1) - dy;
		int d1 = dx << 1;
		int d2 = (dx - dy) << 1;
		int x  = xa;
		int y  = ya + sy;

		image.putPixel ( xa, ya, color );
		for ( int i = 1; i <= dy; i++, y += sy )
		{
			if ( d > 0 )
			{
				d += d2;
				x += sx;
			}
			else
				d += d1;

			image.putPixel ( x, y, color );
		}
	}
}

int main ()
{
	TgaImage	image ( 500, 500 );
	point		p [] = { { 200, 400 }, { 300, 100 }, { 0, 0 } };

	rasterizeTriangle ( image, p, image.rgbToInt ( 0, 0xFF, 0 ) );

	drawLine          ( image, 0,   0,   300, 100, image.rgbToInt ( 0xFF, 0, 0 ) );
	drawLine          ( image, 0,   0,   200, 400, image.rgbToInt ( 0xFF, 0, 0 ) );
	drawLine          ( image, 300, 100, 200, 400, image.rgbToInt ( 0xFF, 0, 0 ) );

	image.writeToFile ( "tri.tga" );

	return 0;
}
