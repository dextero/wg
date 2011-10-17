// zestaw parametrow, ktory moze byc przekazany
// do wzorca przy tworzeniu konkretnego obiektu
#ifndef __CTEMPLATEPARAM_H__
#define __CTEMPLATEPARAM_H__

#include <string>
#include <map>

class CTemplateParam{
public:
    std::map<std::string, void*> values; //@Deprecated, nie castowac na voidy, tylko
                                         //usiasc i porzadnie napisac serializacje/deserializacje
    std::map<std::string, std::string> stringValues;
    std::map<std::string, int> intValues;
};

#endif
