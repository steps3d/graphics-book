//
//
//

#pragma once
#ifndef	__STRING_UTILS__
#define	__STRING_UTILS__

#include	<string>

std::string	stringTrim    ( const std::string& s );
std::string	stringLower   ( const std::string& s );
std::string	stringUpper   ( const std::string& s );
std::string	stringDequote ( const std::string& str );
void	        stringParse   ( const std::string& str, std::string& cmd, std::string& args );
std::string	replaceTabs   ( const std::string& str );
bool            fileExist     ( const std::string& fileName );
std::string	buildFileName ( const std::string& path, const std::string& name );
std::string	getPath       ( const std::string& fullName );
std::string	getFileName   ( const std::string& fullName );
std::string	getName       ( const std::string& fullName );
std::string	getExtension  ( const std::string& fullName );

class Tokenizer
{
    std::string buffer;                      // input string
    std::string separators;                  // characters to split string
    std::string::const_iterator pos;			// string iterator pointing the current position

public:
    Tokenizer () {}
    Tokenizer ( const std::string& str, const std::string& seps = " \t\n\r" );

    void setString    ( const std::string& str, const std::string& seps = " \t\n\r"  );

    std::string next ();                     // return the next token

    bool   atEnd () const
    {
		return pos == buffer.end ();
    }
};

#endif
