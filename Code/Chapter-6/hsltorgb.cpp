#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <math.h>

#define	EPS	0.001f

glm::vec3 rgbToHsl ( const glm::vec3& rgb )
{
	int iMax = 0;     // index of maximum value
	int iMin = 0;     // index of minimum value

	for ( int i = 0; i < 3; i++ )
		if ( rgb [i] > rgb [iMax] )
			iMax = i;
		else
		if ( rgb [i] < rgb [iMin] )
			iMin = i;

	float mx = rgb [iMax];
	float mn = rgb [iMin];
	float d  = mx - mn;

	if ( mx < 0.001 )        // Hue undefined
		return glm::vec3 ( 0 );
  
	float l = (mx + mn) * 0.5;
	float s = d / (1 - fabs(2*l - 1));
	float h;
  
	if ( iMax == 0 )
		h = 60 * (rgb.g - rgb.b) / d;
	else
	if ( iMax == 1 )
		h = 60 * (rgb.b - rgb.r) / d + 120;
	else
		h = 60 * (rgb.r - rgb.g) + 240;

	return glm::vec3 ( h, s, l );
}

glm::vec3 hslToRgb ( const glm::vec3& hsl )
{
  if ( hsl.y < EPS )      // S is zero
    return glm::vec3 ( hsl.z );
  
  float v2  = (hsl.z <= 0.5) ? hsl.z * (1.0 + hsl.y) : 
              (hsl.z + hsl.y - hsl.y * hsl.z);	
  float v1 = 2 * hsl.z - v1;	// XXX

  float      h = hsl.x / 360.0;
  glm::vec3  t ( h + 1.0/3.0, h, h - 1.0/3.0 );

  if ( 6 * t.x < 1 )
    t.x = v1 + (v2-v1)*6.0*t.x;
  else
  if ( 2 * t.x < 1 )
    t.x = v2;
  else
  if ( 3 * t.x < 2 )
    t.x = v1 + (v2-v1)*((2.0/3.0)-t.x)*6.0;
  else
    t.x = v1;
	
  if ( 6 * t.y < 1 )
    t.y = v1 + (v2-v1)*6.0*t.y;
  else
  if ( 2 * t.x < 1 )
    t.y = v2;
  else
  if ( 3 * t.y < 2 )
    t.y = v1 + (v2-v1)*((2.0/3.0)-t.y)*6.0;
  else
    t.y = v1;
	
  if ( 6 * t.z < 1 )
    t.z = v1 + (v2-v1)*6.0*t.z;
  else
  if ( 2 * t.z < 1 )
    t.z = v2;
  else
  if ( 3 * t.z < 2 )
    t.z = v1 + (v2-v1)*((2.0/3.0)-t.z)*6.0;
  else
    t.z = v1;

  return t;	
}
