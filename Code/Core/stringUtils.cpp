//#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include	<algorithm>
#include	<cctype>				// for tolower/toupper
#include	"stringUtils.h"

bool		stringStarts  ( const std::string& str, const std::string& s )
{
	if ( str.length () < s.length () )
		return false;

	return str.substr ( 0, s.length () ) == s;
}

std::string	stringTrim ( const std::string& str )
{
	int	len = (int)str.length ();
	int	i, j;

				// add > 0 for correctly skipping cyrillic
	for ( i = 0; i < len && str [i] > 0 && isspace ( str [i] ); i++ )
		;

	for ( j = len - 1; j >= 0 && str [j] > 0 && isspace ( str [j] ); j-- )
		;

	return str.substr ( i, j - i + 1 );
}

std::string	replaceTabs ( const std::string& str )
{
	size_t		len = str.length ();
	std::string	s   = str;

	for ( size_t i = 0; i < len; i++ )
		if ( s [i] == '\t' )
			s [i] = ' ';

	return s;
}

std::string	stringLower ( const std::string& s )
{
	std::string	str ( s );
	
	transform ( str.begin (), str.end (), str.begin (), ::tolower );
	
	return str;
}

std::string	stringUpper ( const std::string& s )
{
	std::string	str ( s );
	
	transform ( str.begin (), str.end (), str.begin (), ::toupper );
	
	return str;
}

std::string	stringDequote ( const std::string& str )
{
	if ( str [0] == '\"' || str [0] == '\'' )			// remove quotes
		return str.substr ( 1, str.length () - 2 );

	return str;
}

void	stringParse ( const std::string& str, std::string& cmd, std::string& args )
{
	size_t	len = str.length ();
	size_t	pos;

	for ( pos = 0; pos < len && str [pos] != ' ' && str [pos] != '\t'; pos++ )
		;

	cmd  = str.substr ( 0, pos );
	args = replaceTabs ( stringTrim ( str.substr ( pos ) ) );
}

                            // simple check whether the file exists
bool    fileExist ( const std::string& fileName )
{
#ifdef  _WIN32
    struct _stat    buf;

    return ::_stat ( fileName.c_str (), &buf ) != -1;
#else
    struct stat buf;

    return stat ( fileName.c_str (), &buf ) != -1;
#endif
}
							// build filename from path and a name
std::string		buildFileName ( const std::string& path, const std::string& name )
{
#ifdef	_WIN32
	char	pathSeparator = '\\';
#else
	char	pathSeparator = '/';
#endif

	if ( path.empty () )
		return name;

	size_t	pos = path.length () - 1;

	std::string	res ( path );

	if ( path [pos] == '\\' || path [pos] == '/' || path [pos] == ':' )
		res += name;
	else
	{
		res += pathSeparator;
		res += name;
	}

	for ( int i = 0; i < (int)res.length (); i++ )
		if ( res [i] == '/' )
			res [i] = pathSeparator;

	return res;

}
								// extract path from a filename
std::string	getPath ( const std::string& fullName )
{
	for ( int i = (int)fullName.length () - 1; i >= 0; i-- )
	{
		char	ch = fullName [i];

		if ( ch == '\\' || ch == '/' || ch == ':' )
			return fullName.substr ( 0, i + 1 );
	}

	return "";
}
							// extract filename and extension
std::string	getFileName ( const std::string& fullName )
{
	int	len = (int)fullName.length ();

	for ( int i = len - 1; i >= 0; i-- )
	{
		char	ch = fullName [i];

		if ( ch == '\\' || ch == '/' || ch == ':' )
			return fullName.substr ( i + 1 );
	}

	return fullName;
}
							// get only name without extension
std::string	getName ( const std::string& fullName )
{
	std::string	name = getFileName ( fullName );
	size_t	pos  = name.find ( '.' );

	if ( pos == std::string :: npos )
		return name;

	return name.substr ( 0, pos );
}

							// get only extension
std::string	getExtension ( const std::string& fullName )
{
	std::string	name = getFileName ( fullName );
	size_t	pos  = name.find ( '.' );

	if ( pos == std::string :: npos )
		return "";

	return name.substr ( pos + 1 );
}

//////////////////// class Tokenizer ////////////////////////

Tokenizer :: Tokenizer ( const std::string& str, const std::string& seps )  : buffer ( str ), separators ( seps )
{
	pos = buffer.begin ();
}

void Tokenizer :: setString ( const std::string& str, const std::string& seps )
{
	buffer     = str;
	separators = seps;
	pos        = buffer.begin ();
}

std::string	Tokenizer :: next () 
{
    if ( buffer.length () < 1 )				// skip if buffer is empty
		return "";           		

    while ( pos != buffer.end () && separators.find ( *pos ) != std::string::npos )
        ++pos;

	std::string	token;
	
    while ( pos != buffer.end () && separators.find ( *pos ) == std::string::npos )
    {
        token += *pos;
        ++pos;
    }
	
    return token;
}
