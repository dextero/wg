/* Takie fajne 'okienko' co bedzie wyskakiwac graczowi podczas gry gdy
 * dotknie broni, npca, roadsignu, czy czegos innego z czym mozna wejsc
 * w interakcje, i bedzie w miare elastyczne jesli chodzi o zawartosc wyswietlana,
 * i bedzie przechwytywac caly (albo czesc) input danego gracza, aby sterowac
 * w tymze okienku kursorem, czy co... */

#ifndef INTERACTION_TOOLTIP_H
#define INTERACTION_TOOLTIP_H

class InteractionHandler;
namespace GUI {
class CWindow;
};
//class CActor;

class CInteractionTooltip {
public:
	CInteractionTooltip();
	~CInteractionTooltip();

	void Show();
	void Hide();
    void Clear();

    GUI::CWindow * GetCanvas();

//	void Update(float secondsPassed);

    int GetId();

    InteractionHandler * GetHandler();
    void SetHandler(InteractionHandler * handler);
//    void SetPlayer(CPlayer * player);
//    CPlayer * GetPlayer();
    bool IsVisible();

private:
    bool mIsVisible;

    InteractionHandler * mHandler;

    GUI::CWindow * mCanvas;

    int mId;

    // TODO SafePtr!
//    CPlayer * mPlayer; // ja to tak moge sobie przechowywac? Co jak ktos mi zabije tego gracza, wskaznik umrze?

};

#endif
