#ifndef __CLocaleIntegrityChecker_h__
#define __CLocaleIntegrityChecker_h__

#include <map>
#include <string>
#include <vector>

class CLocaleIntegrityChecker
{
public:
    // testuje kazdy jezyk z pierwszym, nie kazdy z kazdym
    bool Check(const std::vector<std::string>& langs);
    bool Check(const std::string& leftFile, const std::string& rightFile);

private:
    struct Entry
    {
        std::string key;
        std::wstring value;
    };
    typedef std::map< std::string, Entry > Dictionary;

    bool Load(const std::string & file, Dictionary & dictionary);
};

#endif // __CLocaleIntegrityChecker_h__
