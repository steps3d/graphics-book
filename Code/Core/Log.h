//
// Basic log class
//
// Author: Alexey V. Boreskov, <steps3d@gmail.com>, <steps3d@narod.ru>
//

#pragma once
#include	<string>
#include	<stdlib.h>
#include	<stdio.h>
#include	<sstream>

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include <glm/matrix.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>


class	Log
{
	std::string			logName;
	std::stringstream	s;
	bool				fatal = false;

public:
	Log  ( const std::string& logFileName ) : logName ( logFileName ) {}
	~Log () {}

	Log&	setLogName ( const std::string& logFileName )
	{
		logName = logFileName;
		

		return *this;
	}
	
	Log& flush ();

	struct endl__ {};
	class fatal__ {};
	
	static endl__ 	endl;
	static Log		appLog;
	
	template <typename T>
	Log& operator << ( T value )
	{
		s << value;

		return *this;
	}

	
	Log& operator << ( endl__ )
	{
		flush ();
		
		if ( fatal )
			::exit ( 1 );
		
		return *this;
	}
	
	Log& operator << ( fatal__ )
	{
		fatal = true;
		
		return *this;
	}

};

Log& log ( int level = 0 );
Log& fatal ();

/*
inline std::ostream& operator << ( std::ostream& stream, const glm::vec2& v )
{ 
	stream << "vec2(" << v.x << ", " << v.y << ")";

	return stream;
}

inline std::ostream& operator << ( std::ostream& stream, const glm::vec3& v )
{ 
	stream << "vec2(" << v.x << ", " << v.y << ", " << v.z << ")";

	return stream;
}

inline std::ostream& operator << ( std::ostream& stream, const glm::vec4& v )
{ 
	stream << "vec2(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";

	return stream;
}

*/
