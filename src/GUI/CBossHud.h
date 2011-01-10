#ifndef BOSSHUD_H
#define BOSSHUD_H

#include <vector>

class CEnemy;

namespace GUI
{
    class CGUIObject;
    class CProgressBar;
    class CImageBox;
    class CTextArea;

    class CBossHud
    {
    public:
        CBossHud();
        ~CBossHud();

        void Init();
        void Update( float dt );
        void Show();
        void Hide();
        void UpdatePosition();

    private:
        void HandleNewBoss(CEnemy * boss);

        CProgressBar *	mHP;
		CImageBox *		mAvatar;
		CGUIObject *	mHud;
        CTextArea *     mText;

        CEnemy * mPreviousBoss;
    };
};

#endif
