#include "StringUtils.h"
#include "CXml.h"

#include <wchar.h>
#include <vector>

namespace StringUtils {

    // charset dla czcionki
std::wstring ValidChars = L"!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                              L" [\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

struct STranslator{
    char a;
    char b;
    wchar_t out;
};

std::vector<STranslator> translators;

void InitializeValidChars(){
    CXml xml("data/unicode-cheatsheet.xml","root");
    STranslator tr;
    std::string s;
    for(xml_node<> *node = xml.GetChild(xml.GetRootNode(),"letter"); node; node = xml.GetSibl(node,"letter")){
        s = xml.GetString(node);
        tr.a = s[0];
        tr.b = s[1];
        tr.out = static_cast<wchar_t>(xml.GetInt(node,"code"));
        translators.push_back(tr);
        ValidChars += tr.out;
    }
}

std::wstring ReinterpretFromUTF8(std::string s){
    std::wstring out(L"");
    out.reserve(s.size());
    bool translated;
    for (unsigned int i = 0; i < s.size(); /* recznie zwieksze i */){
        translated = false;
        if (i < s.size() -1){
            for (unsigned int j = 0; j < translators.size(); j++){
                if ((translators[j].a == s[i]) && (translators[j].b == s[i+1])){
                    translated = true;
                    out += translators[j].out;
                    i+=2;
                    break;
                }
            }
        }
        if (!translated){
            out+=s[i];
            i++;
        }
    }
    return out;
}

std::string TrimWhiteSpaces( const std::string &toTrim )
{
    size_t start = 0, end = 0;

    for(start=0; start < toTrim.length(); start++)
    {
        if(toTrim[start] == ' ' || toTrim[start] == (char)9 || toTrim[start] == (char)10 || toTrim[start] == (char)13)
            continue;
        else
            break;
    }

    for(end=toTrim.length(); end > 0; end--)
    {
        if(toTrim[end-1] == ' ' || toTrim[end-1] == (char)9 || toTrim[end-1] == (char)10 || toTrim[end-1] == (char)13)
            continue;
        else
            break;
    }

    if (start > end)
        return "";

    return toTrim.substr(start, end - start);
}

std::wstring TrimWhiteSpacesW( const std::wstring &toTrim )
{
    size_t start = 0, end = 0;

    for(start=0; start < toTrim.length(); start++)
    {
        if(toTrim[start] == ' ' || toTrim[start] == (char)9 || toTrim[start] == (char)10 || toTrim[start] == (char)13)
            continue;
        else
            break;
    }

    for(end=toTrim.length(); end > 0; end--)
    {
        if(toTrim[end-1] == ' ' || toTrim[end-1] == (char)9 || toTrim[end-1] == (char)10 || toTrim[end-1] == (char)13)
            continue;
        else
            break;
    }

    if (start > end)
        return L"";

    return toTrim.substr(start, end - start);
}

boost::uint64_t GetStringHash( const std::string & str )
{
	const char * data = str.c_str();
	unsigned int len = (unsigned int)str.length();

	boost::uint64_t hash = len, tmp;
	int rem;

	if (len <= 0 || data == NULL) return 0;

	rem = len & 3;
	len >>= 2;

	for (;len > 0; len--)
	{
		hash  += get16bits (data);
		tmp    = (get16bits (data+2) << 11) ^ hash;
		hash   = (hash << 16) ^ tmp;
		data  += 2*sizeof (unsigned short);
		hash  += hash >> 11;
	}

	switch (rem) 
	{
	case 3: hash += get16bits (data);
		hash ^= hash << 16;
		hash ^= data[sizeof (unsigned short)] << 18;
		hash += hash >> 11;
		break;
	case 2: hash += get16bits (data);
		hash ^= hash << 11;
		hash += hash >> 17;
		break;
	case 1: hash += *data;
		hash ^= hash << 10;
		hash += hash >> 1;
	}

	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

    //tox, 9 kwietnia:
	return hash+0x10000000;
}

std::wstring *WSplit(std::wstring &input, wchar_t s, int *out_cnt){
    wchar_t *cstr, *p;
    std::wstring *result;
    size_t count = 1, len = input.size()+1;

    cstr = new wchar_t [len];
    wcscpy (cstr, input.c_str());

    for (unsigned int i = 0; i < len; i++)
        if (cstr[i] == s)
            count++;
    result = new std::wstring[count];

    p = cstr;
    int idx = 0;
    for (unsigned int i = 0; i < len; i++)
        if (cstr[i] == s){
            cstr[i] = '\0';
            result[idx] = *(new std::wstring(p));
            idx++;
            p=cstr + i + 1;
        }
    result[idx] = *(new std::wstring(p));

    if (out_cnt != NULL)
        *out_cnt = (int)count;

    return result;
}

std::string ExtractParam( const std::string & input, const std::string & tag, int words )
{
    size_t posOfTag = input.find( tag );
    if ( posOfTag == std::string::npos )
        return "";

    if ( words == 0 )
    {
        return TrimWhiteSpaces( input.substr( posOfTag + tag.length() ) );
    }

    size_t start = posOfTag + tag.length();
    size_t end = start;
    bool isInWhiteSpace = false;
    if(input[end] == ' ' || input[end] == (char)9 || input[end] == (char)10 || input[end] == (char)13)
        isInWhiteSpace = true;

    for( ; end < input.length(); end++ )
    {
        if(input[end] == ' ' || input[end] == (char)9 || input[end] == (char)10 || input[end] == (char)13)
        {
            if ( isInWhiteSpace )
                continue;
            else
            {
                isInWhiteSpace = true;
                words--;
                if ( words == 0 )
                {
                    size_t length = end - start;
                    return StringUtils::TrimWhiteSpaces( input.substr( start, length ) );
                }
            }
        }
        else
        {
            isInWhiteSpace = false;
        }
    }
    return StringUtils::TrimWhiteSpaces( input.substr( start ) );
}

std::string GenerateCode(std::vector<char> &chars,int maxLength,int hash){
    if (chars.size() > 0){
        int index = hash % chars.size();
        int i = 0;
        std::string result = "";
        while (i < maxLength){
            result += chars[index];
			hash /= (int) chars.size();
            index = hash % chars.size();
            i++;
        }
        return result;
    } else
        return "";
}

std::string GetNextCode_AZ(const std::string& lastCode)
{
    std::string ret = lastCode;

    size_t i;
    for (i = 0; i < ret.size(); ++i)
    {
        if (ret[i] == 'z')
            ret[i] = 'a';
        else
        {
            ret[i] = ret[i] + 1;
            break;
        }
    }
    // jesli wszystkie znaki byly 'z'kami, to zamieni je na 'a' i dopisze na koniec jeszcze jedno 'a'
    if (i == ret.size())
        ret += "a";

    return ret;
}

void Exclude( const std::string& s, char ch, std::vector<std::string>& out )
{
	std::string curWord;
	out.clear();

	for ( unsigned i = 0; i < s.size(); i++ )
	{
		if ( s[i] == ch && curWord != "" )
		{
			out.push_back( curWord );
			curWord = "";
		}
		else
			curWord.push_back( s[i] );
	}

	if ( curWord != "" )
		out.push_back( curWord );
}

std::string ToUpper(const std::string& s)
{
    std::string str;
    std::string::const_iterator i = s.begin();
    std::string::const_iterator end = s.end();

    // zamiana na wielkie litery
    while (i != end) {
        str += (*i >= 'a' && *i <= 'z' ? *i - 'a' + 'A' : *i);
        ++i;
    }

    return str;
}


std::string FloatToString(float num, unsigned precision)
{
    std::ostringstream streamOut;
    streamOut.precision(precision);
    streamOut << std::fixed << num;
    std::string ret = streamOut.str();
    size_t lastNonZero = ret.find_last_not_of(".0") + 1;
    return ret.substr(0, std::max(lastNonZero ? lastNonZero : 1, ret.size() - precision - 1));
}

std::wstring FloatToWString(float num, unsigned precision)
{
    std::wostringstream streamOut;
    streamOut.precision(precision);
    streamOut << std::fixed << num;
    std::wstring ret = streamOut.str();
    size_t lastNonZero = ret.find_last_not_of(L".0") + 1;
    return ret.substr(0, std::max(lastNonZero ? lastNonZero : 1, ret.size() - precision - 1));
}

std::vector< std::string > Tokenize( const std::string & input, char separator )
{
    std::vector< std::string > ret;
    size_t separatorPosition = 0;
    do
    {
        size_t nextSeparatorPosition = input.find( separator, separatorPosition + 1 );
        size_t length = nextSeparatorPosition - separatorPosition;
        ret.push_back( input.substr( separatorPosition, length ) );
        separatorPosition = nextSeparatorPosition;
        if ( separatorPosition != std::string::npos)
            separatorPosition++;
    }
    while ( separatorPosition != std::string::npos );

    return ret;
}

std::vector<std::string> Explode(const std::string& str, const std::string& separator)
{
	std::vector<std::string> ret;

	if (str.size() > 0 && separator.size() > 0)
	{
		size_t prev = 0, at = str.find(separator, 0);
		for (; prev != std::string::npos; at = str.find(separator, at + 1))
		{
			ret.push_back(str.substr(prev, at - prev));
			prev = at;
			if (prev != std::string::npos)
				prev += separator.size();
		}
	}

	return ret;
}

const std::string ReplaceAllOccurrences(const std::string& str, const std::string& replaceWhat, const std::string& replaceTo)
{
	std::stringstream ret;

	if (str.size() > 0 && replaceWhat.size() > 0)
	{
		size_t prev = 0, at = str.find(replaceWhat, 0);
		for (; prev != std::string::npos; at = str.find(replaceWhat, at + 1))
		{
			if (at - prev > replaceWhat.size())
				ret << str.substr(prev, at - prev);

			prev = at;

			if (prev != std::string::npos)
			{
				ret << replaceTo;
				prev += replaceWhat.size();
			}
		}
	}

	return ret.str();
}

} // namespace StringUtils
