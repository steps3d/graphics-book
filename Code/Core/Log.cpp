//
// Basic log class
//
// Author: Alexey V. Boreskov, <steps3d@gmail.com>, <steps3d@narod.ru>
//

#include	<iostream>
#include	<fstream>
#include	"Log.h"

#ifdef	_WIN32
	#include	<windows.h>
#endif

//static	Log	appLog ( "" );		// create application log
Log::endl__	Log::endl;			// creat end-of-line marker

Log& Log::flush ()
{
	std::string	temp = s.str ();	// get string from stream
		
	s.str ( std::string () );		// clear stream
	
//	temp += '\n';
	
	puts ( temp.c_str () );
	
#ifdef	_WIN32
	OutputDebugString ( temp.c_str () );
#endif
	if ( !logName.empty () )
	{
		std::ofstream file;

		file.open ( logName, std::ios::app );
		file << temp << std::endl;
	}
		
//assert (_heapchk () == _HEAPOK );

	return *this;
}

Log& log ( int level )
{
	static	Log	appLog2 ( "" );		// create application log

//assert (_heapchk () == _HEAPOK );

	return appLog2;
}

Log& fatal ()
{
	//return appLog << Log::fatal__ ();
	return log () << Log::fatal__ ();
}
