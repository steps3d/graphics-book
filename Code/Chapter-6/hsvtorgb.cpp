#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <math.h>

#define	EPS	0.001f

glm::vec3 rgbToHsv ( const glm::vec3& rgb )
{
	int iMax = 0;     		// index of maximum value
	int iMin = 0;     		// index of minimum value
  
	for ( int i = 0; i < 3; i++ )
		if ( rgb [i] > rgb [iMax] )
			iMax = i;
		else
		if ( rgb [i] < rgb [iMin] )
			iMin = i;
	  
	float mx = rgb [iMax];
	float mn = rgb [iMin];
	float d  = mx - mn;

	if ( mx < EPS )        // Hue undefined
		return glm::vec3 ( 0 );

	float v = mx;
	float s = d / mx;
	float h;

	if ( iMax == 0 )
		h = 60 * (rgb.g - rgb.b) / d;
	else
	if ( iMax == 1 )
		h = 60 * (rgb.b - rgb.r) / d + 120;
	else
		h = 60 * (rgb.r - rgb.g) + 240;

	return glm::vec3 ( h, s, v );
}

glm::vec3 hsvToRgb ( const glm::vec3& hsv )
{
	float v = hsv.z;
	
	if ( hsv.y < 0.001 )
		return glm::vec3 ( v );

	float h  = hsv.x * 6.0;
	float fi = floor ( h );
	int   i  = int ( fi );
	float f  = h - fi;
	float p  = hsv.z * (1.0 - hsv.y);
	float q  = hsv.z * (1.0 - hsv.y * f);
	float t  = hsv.z * (1.0 - hsv.y * (1.0 - f));

	if ( i == 0 )
		return glm::vec3 ( v, t, p );
	else
	if ( i == 1 )
		return glm::vec3 ( q, v, p );
	else
	if ( i == 2 )
		return glm::vec3 ( p, v, t );
	else
	if ( i == 3 )
		return glm::vec3 ( p, q, v );
	else
	if ( i == 4 )
		return glm::vec3 ( t, p, v );
	else
		return glm::vec3 ( v, p, q );
}

glm::vec3 sinCityEffect ( const glm::vec3& color )
{
	glm::vec3	hsv = rgbToHsv ( color );
	float		delta = 10;
	
			// check whether it is red
	if ( hsv.x < delta || hsv.x > 360 - delta )
		hsv.y *= 1.5f;
	else
		hsv.y /= 1.5f;
	
	return hsvToRgb ( hsv );
}
