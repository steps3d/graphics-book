//
// Basic log class
//
// Author: Alexey V. Boreskov, <steps3d@gmail.com>, <steps3d@narod.ru>
//

#include	"Log.h"

#ifdef	_WIN32
#include	<windows.h>
#endif

Log :: Log ( const char * logFileName ) : str ()
{
	if ( logFileName != nullptr )
		logName = logFileName;
}

Log :: ~Log ()
{

}

Log& Log :: flush ()
{
	puts ( str.c_str () );

	if ( !logName.empty () )
	{
		FILE * fp = fopen ( logName.c_str (), "at" );

		if ( fp != nullptr )
		{
			str += '\n';

			fputs  ( str.c_str (), fp );
			fclose ( fp );
		}
	}

#ifdef	_WIN32
	OutputDebugString ( str.c_str () );
#endif
	str.clear ();

	return *this;
}
