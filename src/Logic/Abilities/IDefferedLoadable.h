#ifndef __IDEFFEREDLOADABLE_H__
#define __IDEFFEREDLOADABLE_H__

/**
 * Liosan, 09.11:
 * wspolny interfejs dla obiektow skladowych efektow logicznych, 
 * ktore musze byc ladowane z odroczeniem
 */
class CXml;
namespace rapidxml{
    template<class Ch> class xml_node;
}

class IDefferedLoadable{
public:
    IDefferedLoadable(){};
    virtual ~IDefferedLoadable(){};
    
    virtual void Load(CXml &xml, rapidxml::xml_node<char> *root = 0) = 0;
};

#endif /* __IDEFFEREDLOADABLE_H__ */

