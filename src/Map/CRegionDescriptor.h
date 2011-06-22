#ifndef __REGIONDESCRIPTOR_H__
#define __REGIONDESCRIPTOR_H__

class CCondition;

#include <string>
#include <ostream>
#include <SFML/System/Vector2.hpp>
#include "../Utils/CXml.h"

class CCondition;
class CDynamicRegion;
class CEffectHandle;

namespace Map{
    
    class CRegionDescriptor{
    public:
        std::string name;
        const sf::Vector2f pos;
        CCondition *cond;
        std::string nextMap;
        std::string nextMapRegion;
        float rot, scale;
        CEffectHandle *effect;
        int uniqueId;

        CRegionDescriptor(const std::string &_name, const sf::Vector2f _pos, float _scale = 1.f);
        virtual ~CRegionDescriptor();

        CDynamicRegion *Create();
        void Load(CXml &xml, xml_node<> *node);
        void Serialize(std::ostream &out, int indent);
        const std::string Serialize();

#ifdef __EDITOR__
        friend class CEditorWnd;
#endif
    };

}
#endif // __REGIONDESCRIPTOR_H__
