/* Takie fajne 'okienko' co bedzie wyskakiwac graczowi podczas gry gdy
 * dotknie broni, npca, roadsignu, czy czegos innego z czym mozna wejsc
 * w interakcje, i bedzie w miare elastyczne jesli chodzi o zawartosc wyswietlana,
 * i bedzie przechwytywac caly (albo czesc) input danego gracza, aby sterowac
 * w tymze okienku kursorem, czy co... */

#ifndef INTERACTION_TOOLTIP_H
#define INTERACTION_TOOLTIP_H

#include "CButton.h"
#include "../IFrameListener.h"

#include <string>
#include <vector>
#include <SFML/Graphics/Color.hpp>

class IOptionChooserHandler;
class CActor;

namespace GUI { class CButton; }


class CInteractionTooltip {
public:
	CInteractionTooltip();
	~CInteractionTooltip();

    void SetTitle(const std::string & title);

	void Show();
	void Hide();

	void Update(float secondsPassed);

    void SetOptionHandler(IOptionChooserHandler * handler);
    void SetPlayer(CPlayer * player);
    CPlayer * GetPlayer();
    bool IsVisible();

private:
    bool mIsVisible;
    std::string mTitle;

    // TODO SafePtr!
    CPlayer * mPlayer; // ja to tak moge sobie przechowywac? Co jak ktos mi zabije tego gracza, wskaznik umrze?

};

#endif
