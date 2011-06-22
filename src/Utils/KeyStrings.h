#ifndef KEY_STRINGS_H
#define KEY_STRINGS_H

#include <string>

/*	Numeracja klawiszy taka jak w BindManagerze, np. prawy shift to sf::Key::RShift a ppm to sf::Key::Count + sf::Mouse::Right 
*	KeyToString = "" | StringToKey = -1  <--  nie znaleziono klawisza
*/

namespace KeyStrings
{
	std::string	KeyToString( int key );
	int			StringToKey( const std::string& s );

    const std::wstring GetBindDesc( const std::string& s);
};

#endif