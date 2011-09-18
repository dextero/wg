#ifndef HUD_H
#define HUD_H

#include <vector>

namespace GUI
{
	class CGUIObject;
	class CProgressBar;
	class CImageBox;
	class CTextArea;

	/* Czas przez jaki ma byc sygnalizowany na hudzie brak many */
	const float maxManaFlaringTime = 0.3f;

	class CHud
	{
	public:
		CHud();
		~CHud();

		void Init(size_t playerNumber);
		void Update( float dt );
		void Show();
		void Hide();
        void UpdatePosition();

		//inline void ResetSequences() { mSeq.clear(); }
	private:
		size_t  		mPlayerNumber;
		CGUIObject *	mHud;
		CProgressBar *	mHP, *mMana, *mManaFlr, *mXP;
		CImageBox *		mAvatar, *mHudFg;
		CTextArea *		mHudFGText;

		float mManaFlaringTime;
	};
};

#endif