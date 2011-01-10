#ifndef __CDROPMANAGER_H__
#define __CDROPMANAGER_H__

#include "../../Utils/CSingleton.h"
#include <SFML/System/Vector2.hpp>
#include <string>

class CItem;
class CItemTemplate;

#define gDropManager CDropManager::GetSingleton()

/// 4.05.09, rAum: klasa zajmujaca sie tworzeniem itemow
/// ktore wypadaja z potwora.
/// Algorytm losowania jest banalny (bardzo zwykle losowanie ze wzrostem prawdopodobienstwa wylosowania czegos :P).
/// TODO: skomplikowac algorytm losowania i zmienic z hardcoded na data driven.
// Liosan, 06.07.09: sam jestes skomplikowac, ma byc elastyczny a nie skomplikowany ;)
class CDropManager : public CSingleton<CDropManager>
{
public:
    void Clear();

    bool LoadItems(const std::string &filename);
    
    /// zwraca przedmiot lub NULL jesli nic nie ma "wypasc"
    CItem* DropItemAt(const sf::Vector2f & position, unsigned maxLvlItem=10);
private:
    std::vector<CItemTemplate *> dropItemTemplates;
};

#endif //__CDROPMANAGER_H__//
