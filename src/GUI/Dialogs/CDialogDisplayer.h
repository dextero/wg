#ifndef _DIALOG_DISPLAYER_
#define _DIALOG_DISPLAYER_

#include "../../IKeyListener.h"
#include "../../IFrameListener.h"
#include "../../Utils/CSingleton.h"
#include <vector>
#include <string>

#define gDialogDisplayer GUI::CDialogDisplayer::GetSingleton()

class	CDialogGraph;
struct	SDialogNode;

namespace GUI
{
	class	CWindow;
	class	CTextArea;


	class CDialogDisplayer : public IFrameListener, public IKeyListener, public CSingleton< CDialogDisplayer >
	{
		enum dialogState
		{
			GRAPH_EMPTY = 0,
			GRAPH_SEARCHING,
			STATEMENT_DISPLAY,
			OPTION_CHOICE
		};

	public:
		CDialogDisplayer();
		~CDialogDisplayer();

        bool GetDisplaying() { return mCurrentDialog != NULL; };
		void DisplayDialog( CDialogGraph* dialog );
		void ExitDialog();
		
		virtual void KeyPressed( const sf::Event::KeyEvent& e );
		virtual void FrameStarted( float secondsPassed );
		virtual bool FramesDuringPause( EPauseVariant pv ) { return pv == pvLogicOnly; }

	private:
        const std::wstring ParametrizeText(const std::wstring& text, const std::vector<std::string>& parameters);

		void DisplayStatement( SDialogNode* statement );
		void HideStatement();

		void DisplayOptions( const std::vector<SDialogNode*>& options );
		void SelectOption( unsigned int selected );
		void ChooseSelectedOption();

		void SetCurrentNode( SDialogNode* node );
		bool IsNodeAvailable( SDialogNode* node );

		CDialogGraph* mCurrentDialog;
		SDialogNode*  mCurrentNode;
		dialogState   mState;

		CTextArea*	  mStatementArea;
		//float		  mTimeToHideStatement; // dex: wylaczone

		CWindow*	  mOptionsArea;
		unsigned int  mSelectedOption;
		std::vector<CTextArea*>		mOptionTexts;	/* i+1-te pole tekstowe jest dzieckiem i-tego */
		std::vector<SDialogNode*>	mOptionNodes;
	};
};

#endif

