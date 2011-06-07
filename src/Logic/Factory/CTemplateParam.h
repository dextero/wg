// zestaw parametrow, ktory moze byc przekazany
// do wzorca przy tworzeniu konkretnego obiektu
#ifndef __CTEMPLATEPARAM_H__
#define __CTEMPLATEPARAM_H__

#include <string>
#include <map>

class CTemplateParam{
public:
    std::map<std::string, void*> values;
    std::map<std::string, std::string> stringValues;
};

#endif
