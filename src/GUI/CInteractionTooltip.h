/* Takie fajne 'okienko' co bedzie wyskakiwac graczowi podczas gry gdy
 * dotknie broni, npca, roadsignu, czy czegos innego z czym mozna wejsc
 * w interakcje, i bedzie w miare elastyczne jesli chodzi o zawartosc wyswietlana,
 * i bedzie przechwytywac caly (albo czesc) input danego gracza, aby sterowac
 * w tymze okienku kursorem, czy co... */

#ifndef INTERACTION_TOOLTIP_H
#define INTERACTION_TOOLTIP_H

#include <string>

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

    int GetId();

    InteractionHandler * GetHandler();
    void SetHandler(InteractionHandler * handler);
    bool IsVisible();
    void OptionSelected(size_t selected);

    void SetPriority(int priority);
    int GetPriority();

private:
    bool mIsVisible;

    InteractionHandler * mHandler;

    GUI::CWindow * mCanvas;

    int mId;

    int mPriority;
};

#endif
