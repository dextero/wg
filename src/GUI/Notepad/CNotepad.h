#include "../../Utils/CSingleton.h"
#include <string>
#include <vector>

#define gNotepad GUI::CNotepad::GetSingleton()

namespace GUI
{
	class CWindow;
	class CButton;
	class CScrollBar;
	class CScrollPanel;
	class CTextArea;

	class CNotepad : public CSingleton< CNotepad >
	{
	public:
		CNotepad();
		~CNotepad();

		void Show( const std::wstring& page = L"" );
		void Hide();
		void AddNote( const std::wstring& page, const std::wstring& note );

        void ResetGuiControls();
        void Init();

	private:

		struct SPage
		{
			CButton*				  caption_btn;
			std::wstring			  caption;
			std::vector<std::wstring> notes;
		};

		SPage* FindPage( const std::wstring& caption );
		SPage* AddPage( const std::wstring& caption );

		CWindow*		mNotepadWnd;
		CScrollPanel*	mCaptionsPanel;
		CScrollBar*		mCaptionsScrollBar;
		CTextArea*		mPageContent;
		CScrollBar*		mPageScrollBar;

		std::vector<SPage> mPages;
	};
};