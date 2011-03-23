#ifndef __CLOOTMANAGER_H__
#define __CLOOTMANAGER_H__

#include "../../Utils/CSingleton.h"
#include <SFML/System/Vector2.hpp>
#include <string>

class CLoot;
class CLootTemplate;

#define gLootManager CLootManager::GetSingleton()

/// 4.05.09, rAum: klasa zajmujaca sie tworzeniem znajdziek
/// ktore wypadaja z potwora.
/// Algorytm losowania jest banalny (bardzo zwykle losowanie ze wzrostem prawdopodobienstwa wylosowania czegos :P).
/// TODO: skomplikowac algorytm losowania i zmienic z hardcoded na data driven.
// Liosan, 06.07.09: sam jestes skomplikowac, ma byc elastyczny a nie skomplikowany ;)
class CLootManager : public CSingleton<CLootManager>
{
public:
    void Clear();

    bool LoadLoots(const std::string &filename);
    
    /// zwraca przedmiot-znajdzke lub NULL jesli nic nie ma "wypasc"
    CLoot* DropLootAt(const sf::Vector2f & position, unsigned maxLvlLoot=10);
private:
    std::vector<CLootTemplate *> mLootTemplates;
};

#endif //__CLOOTMANAGER_H__//
