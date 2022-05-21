//
// Basic class to handle reading of static data
//
// Author: Alexey V. Boreskov
//

#pragma once
#ifndef	__DATA__
#define	__DATA__

#ifdef	_WIN32
	#pragma	warning (disable:4786 4996)
#endif

#include	<string>
#include	<stdint.h>

class	Data
{
	uint8_t  * bits;
	int		length;
	int		pos;
	std::string	file;						// when data loaded from file, contains it's name

public:
	explicit Data ( const std::string& fileName );
	Data ( void * ptr, int len );
	~Data ();

	bool	isOk () const;

	const std::string&	getFileName () const
	{
		return file;
	}
	
	bool	isEmpty () const
	{
		return pos >= length;
	}

	int	getLength () const
	{
		return length;
	}

	int	getPos () const
	{
		return pos;
	}

	int	getByte ()
	{
		if ( pos < length )
			return bits [pos++];
		
		return -1;
	}

	int16_t	getShort ()
	{
		if ( pos + 1 >= length )
			return -1;

		int16_t 	v = *(int16_t *) (bits + pos);

		pos += 2;

		return v;
	}

	uint16_t	getUnsignedShort ()
	{
		if ( pos + 1 >= length )
			return -1;

		uint16_t v = *(uint16_t *) (bits + pos);

		pos += 2;

		return v;
	}

	int32_t getLong ()
	{
		if ( pos + 3 >= length )
			return -1;

		int32_t	v = *(int32_t *) (bits + pos);

		pos += 4;

		return v;
	}

	uint32_t getUnsignedLong ()
	{
		if ( pos + 3 >= length )
			return -1;

		uint32_t v = *(uint32_t *) (bits + pos);

		pos += 4;

		return v;
	}

	float	getFloat ()
	{
		if ( pos + 3 >= length )
			return 0;

		float f = *(float *)(bits + pos);

		pos += 4;

		return f;
	}

	double	getDouble ()
	{
		if ( pos + 7 >= length )
			return 0;

		double d = *(double *)(bits + pos);

		pos += 8;

		return d;
	}

	void * getPtr () const
	{
		return bits + pos;
	}

	void * getPtr ( int offs ) const;

	int	seekCur ( int delta )
	{
		pos += delta;

		if ( pos > length )
			pos = length;

		if ( pos < 0 )
			pos = 0;

		return pos;
	}

	int	seekAbs ( int offs )
	{
		pos = offs;

		if ( pos > length )
			pos = length;

		if ( pos < 0 )
			pos = 0;

		return pos;
	}

	std::string	getString ()
	{
		std::string	str;
		
		if ( !getString ( str ) )
			return "";
		
		return str;
	}
	
	int		getBytes   ( void * ptr, int len );
	bool	getString  ( std::string& str, char term = '\0' );		// get a string with given terminator
	bool	saveToFile ( const char * name ) const;
	void	dump       ( int num, int offs = 0 ) const;
};

#endif
