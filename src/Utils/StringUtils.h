#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include <string>
#include <sstream>
#include <vector>

#include <boost/cstdint.hpp>

// przyspieszenie porównywania 8-znakowych ci¹gów - rozpatrzyæ u¿ycie
//union StringKey
//{    
//    unsigned long long id;
//    char[sizeof(unsigned long long)] cd;
//};

namespace StringUtils {

void InitializeValidChars();

#if !defined (get16bits)
#define get16bits(d) ((((unsigned int)(((const unsigned char *)(d))[1])) << 8)+(unsigned int)(((const unsigned char *)(d))[0]) )
#endif

    // charset dla czcionki
extern std::wstring ValidChars;

std::string TrimWhiteSpaces( const std::string &toTrim );
std::wstring TrimWhiteSpacesW( const std::wstring &toTrim );
//http://www.azillionmonkeys.com/qed/hash.html
boost::uint64_t GetStringHash( const std::string &str );

inline std::string ConvertToString( std::wstring ws )
{
    return std::string( ws.begin(), ws.end() );
}

inline std::wstring ConvertToWString( std::string s )
{
    return std::wstring( s.begin(), s.end() );
}

std::wstring ReinterpretFromUTF8(std::string s);

/////////////////////////////////////
// http://articles.techrepublic.com.com/5100-10878_11-5079969.html#
// the 'to_string' function
template< class type>
inline std::string ToString( const type & value)
{
    std::ostringstream streamOut;
    streamOut << value;
    return streamOut.str();
}

template< class type>
inline std::wstring ToWString( const type & value)
{
    std::wostringstream streamOut;
    streamOut << value;
    return streamOut.str();
}

template< class type>
inline bool FromString( const std::string& str, type& value )
{
	std::istringstream streamIn( str );
	return !( streamIn >> value).fail();
}

// tox, 10 czerwca, wersja 'funkcyjna':
template< class type>
inline type Parse( const std::string& str )
{
	std::istringstream streamIn( str );
    type ret;
    streamIn >> ret;
    return ret;
}

template< class type>
inline type Parse( const std::wstring& str )
{
	std::wistringstream streamIn( str );
    type ret;
    streamIn >> ret;
    return ret;
}

// kodowanie-dekodowanie
inline int CharToCode(char c){
    return c - 'a';
}

// jak komus sie chce moze napisac druga :)
std::wstring *WSplit(std::wstring &input, wchar_t s, int *out_cnt);

std::string ExtractParam( const std::string & input, const std::string & tag, int words = 1 );

std::vector< std::string > Tokenize( const std::string & input, char separator = ',' );

std::string GenerateCode(std::vector<char> &chars,int maxLength,int hash);
std::string GetNextCode_AZ(const std::string& lastCode);

void Exclude( const std::string& s, char ch, std::vector<std::string>& out );

std::string ToUpper(const std::string& s);

std::string FloatToString(float num, unsigned precision);
std::wstring FloatToWString(float num, unsigned precision);

std::vector<std::string> Explode(const std::string& str, const std::string& separator);
const std::string ReplaceAllOccurrences(const std::string& str, const std::string& replaceWhat, const std::string& replaceTo);
}

// hack - kompatybilnosc minGW - g++
#ifdef _WIN32
#define vswprintf( buf, len, text, args) _vsnwprintf( buf, len, text, args )
#endif

#endif//__STRING_UTIlS_H__//

