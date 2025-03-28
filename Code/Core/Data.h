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
	struct	SharedBits
	{
		uint8_t  * bits = nullptr;
		int		length   = 0;
		std::string	file;		// when data loaded from file, contains it's name
		int		refCount = 0;

		SharedBits  ( const std::string& fileName );
		~SharedBits ();

		void	release ()
		{
			if ( --refCount < 1 )
			{
				free ( bits );
				bits = nullptr;
			}
		}
	};

	SharedBits    * shared = nullptr;
	uint8_t  * bits = nullptr;
	int		length   = 0;
	int		pos;

public:
	explicit Data ( const std::string& fileName );
	Data ( SharedBits *, int offs, int len );
	Data ( void * ptr, int len );
	~Data ();

	bool	isOk () const;

	const std::string	getFileName () const
	{
		return shared ? shared->file : "";
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

	template<typename T>
	T get ()
	{
		if ( pos + sizeof(T) - 1 >= length )
			return {};

		T v = *(T *)(bits + pos);

		pos += sizeof ( T );

		return v;
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

	int	skip ( int delta )
	{
		return seekCur ( delta );
	}

	std::string	getString ()
	{
		std::string	str;
		
		if ( !getString ( str ) )
			return "";
		
		return str;
	}

	Data	subData ( int offset, int size )
	{
		return Data ( shared, offset, size );
	}
	
	int		getBytes   ( void * ptr, int len );
	bool	getString  ( std::string& str, char term = '\0' );		// get a string with given terminator
	bool	saveToFile ( const char * name ) const;
	void	dump       ( int num, int offs = 0 ) const;
};

#endif
