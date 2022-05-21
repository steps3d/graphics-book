//
// Basic class to handle reading of static data
//
// Author: Alex V. Boreskoff
//

#include	<stdio.h>

#ifdef	_WIN32
    #include	<fcntl.h>
    #include	<io.h>
    #include	<sys/stat.h>
#else
    #include	<unistd.h>
    #include	<sys/types.h>
    #include	<sys/stat.h>
    #include	<fcntl.h>
    #define	O_BINARY	0
#endif

#include	<malloc.h>
#include	<memory.h>
#include	<string.h>
#include	"Data.h"

Data :: Data ( void * ptr, int len )
{
	bits   = (uint8_t *) ptr;
	length = len;
	pos    = 0;
}

Data :: Data ( const std::string& fileName )
{										// make a fix for windows to replace '/' in file path
												// to windoze style '\\' if under windoze
	std::string name ( fileName );
#ifdef	_WIN32
	size_t	    i;

	while ( ( i = name.find ( '/' ) ) != std::string::npos )
		name [i] = '\\';
#endif
	
	bits   = nullptr;
	length = 0;
	pos    = 0;
	file   = name;

	int	fd = open ( name.c_str (), O_RDONLY | O_BINARY );

	if ( fd == -1 )
		return;

#ifndef _WIN32
	struct	stat statBuf;
	
	fstat ( fd, &statBuf );
	
	long	len = statBuf.st_size; 
#else	
	long	len = filelength ( fd );
#endif

	if ( len == -1 )
	{
		close ( fd );

		return;
	}

	bits = (uint8_t *) malloc ( len + 1 );	// add '\0' at the end of data

	if ( bits == nullptr )
	{
		close ( fd );

		return;
	}

	length     = read ( fd, bits, len );
	bits [len] = 0;

	close ( fd );
}

Data :: ~Data ()
{
	if ( !file.empty () )
		free ( bits );
}
bool	Data :: isOk () const
{
	return bits != nullptr;
}

void * Data :: getPtr ( int offs ) const
{
	if ( offs < 0 || offs >= length )
		return nullptr;

	return bits + offs;
}

int	Data :: getBytes ( void * ptr, int len )
{
	if ( pos >= length )
		return -1;

	if ( pos + len > length )
		len = length - pos;

	memcpy ( ptr, bits + pos, len );

	pos += len;

	return len;
}

bool	Data ::  getString ( std::string& str, char term )
{
	if ( pos >= length )
		return false;

	str = "";

	while ( pos < length && bits [pos] != term )
		str += bits [pos++];

	if ( pos < length && bits [pos] == term )
		pos ++;
													// skin OA part of line terminator (0D,0A)
	if ( term == '\r' && pos + 1 < length && bits [pos+1] == '\n' )
		pos++;

	return true;
}

bool	Data :: saveToFile ( const char * name ) const
{
	int	fd = open ( name, O_WRONLY | O_BINARY | O_CREAT | O_TRUNC, S_IWRITE );

	if ( fd == -1 )
		return false;

	write ( fd, bits, length );
	close ( fd );

	return true;

}
void	Data :: dump ( int num, int offs ) const
{
	uint8_t * ptr = offs + (uint8_t *) bits;

	for ( int i = 0; i < num; i++ )
		printf ( "%02x ", ptr [i] );

	printf ( "\n" );
}

