#ifndef HUD_H
#define HUD_H

#include <vector>
#include <string>

struct SHudDesc;

namespace GUI
{
	class CGUIObject;
	class CProgressBar;
	class CImageBox;
	class CTextArea;

	/* Klasa bazowa HUDa, posiada metode statyczna CreateHud, ktora wybiera wlasciwa implementacje
	 * na podstawie danych wczytanych z xmla gracza */

	class CHud
	{
	public:
		CHud(unsigned playerNumber, SHudDesc& hudDesc);
		virtual ~CHud();
		virtual void Update(float dt) = 0;
		virtual void Show();
		virtual void Hide();
		virtual void Release(); // usuwa obiekt wraz z kontrolkami GUI (w przeciwienstwie do 'delete hud;')

		static CHud* CreateHud(unsigned playerNumber, SHudDesc& hudDesc);

	protected:
		unsigned	mPlayerNumber;
		std::string mGuiNamePrefix;
		CGUIObject*	mHud;		
	};

	/* ClassicHud - prosty iphone'owy hud - 3 poziome progress bary */

	class CClassicHud : public CHud
	{
	public:
		CClassicHud(unsigned playerNumber, SHudDesc& hudDesc);
		virtual ~CClassicHud();
		virtual void Update(float dt);

	private:
		CProgressBar* mHP;
		CProgressBar* mMana;
		CProgressBar* mXP;
	};

	/* ModernHud - polokragle progress bary, avatar etc */

	class CModernHud : public CHud
	{
	public:
		CModernHud(unsigned playerNumber, SHudDesc& hudDesc);
		virtual ~CModernHud();
		virtual void Update( float dt );
		virtual void Show();

	private:
		CProgressBar* mHP;
		CProgressBar* mMana;
		CProgressBar* mManaFlr;
		CProgressBar* mXP;
		CImageBox* mAvatar;
		CImageBox* mHudFg;
		CTextArea* mHudFGText;
		std::vector<CImageBox*>	mSeqViewer;
		std::vector<int>		mSeq;

		float mManaFlaringTime;
	};
};

#endif