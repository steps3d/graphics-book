//
// Basic log class
//
// Author: Alexey V. Boreskov, <steps3d@gmail.com>, <steps3d@narod.ru>
//

#pragma once
#include	<string>
#include	<stdlib.h>
#include	<stdio.h>

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include <glm/matrix.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#ifndef	_CVTBUFSIZE
	#define	_CVTBUFSIZE	80
#endif

//using namespace std;

class	Log
{
	std::string	str;
	std::string	logName;

public:
	Log  ( const char * logFileName );
	~Log ();

	Log& append ( const char * s )
	{
		str += s;

		return *this;
	}

	Log& append ( const std::string& s )
	{
		str += s;

		return *this;
	}

	Log& flush ();

	class	endl
	{
	};

};

inline	Log& operator << ( Log& log, Log::endl )
{
	return log.flush ();
}

inline	Log& operator << ( Log& log, const char * str )
{
	return log.append ( str );
}

inline	Log& operator << ( Log& log, const std::string& str )
{
	return log.append ( str );
}

inline	Log& operator << ( Log& log, int n )
{
	char	buf [32];

#ifdef	_WIN32
	itoa ( n, buf, 10 );
#else
	sprintf ( buf, "%d", n );
#endif

	return log.append ( buf );
}


inline Log& operator << ( Log& log, float f )
{
	char	buf [_CVTBUFSIZE];

	return log.append ( gcvt ( f, 12, buf ) );
}

inline Log& operator << ( Log& log, double f )
{
	char	buf [_CVTBUFSIZE];

	return log.append ( gcvt ( f, 12, buf ) );
}

inline Log& operator << ( Log& log, const glm::vec2& v )
{
	return log << "( " << v.x << ", " << v.y << ") ";
}

inline Log& operator << ( Log& log, const glm::vec3& v )
{
	return log << "( " << v.x << ", " << v.y << ", " << v.z << ") ";
}

inline Log& operator << ( Log& log, const glm::vec4& v )
{
	return log << "( " << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ") ";
}
