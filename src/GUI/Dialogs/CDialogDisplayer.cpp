#include "CDialogDisplayer.h"
#include "../../CGame.h"
#include "../../Logic/CDialogGraph.h"
#include "../../Logic/Quests/CQuestManager.h"
#include "../../Logic/Quests/CQuestEvent.h"
#include "../../Logic/CEquipment.h"
#include "../../Utils/StringUtils.h"
#include "../CRoot.h"
#include "../CTextArea.h"
#include "../CWindow.h"
#include "../../GUI/Localization/CLocalizator.h"
#include "../../Input/CBindManager.h"
#include "../../Utils/KeyStrings.h"
#include <iostream>

#undef CreateWindow

using namespace GUI;
using namespace std;

template<> CDialogDisplayer* CSingleton< CDialogDisplayer >::msSingleton = NULL;

/* PUBLIC METHODS: --------------------------------------------*/

CDialogDisplayer::CDialogDisplayer()
:	mCurrentDialog( NULL ),
	mCurrentNode( NULL ),
	mState( GRAPH_EMPTY )
{
	mStatementArea = gGUI.CreateTextArea( "dialog-statement", true, Z_GUI2 );
	mStatementArea->SetPosition( 5.0f, 5.0f, 90.0f, 0.0f );
	mStatementArea->SetPadding( 20.0f, 20.0f, 20.0f, 20.0f, UNIT_PIXEL );
	mStatementArea->SetAutoHeight( true );
	mStatementArea->SetFont(gGUI.GetFontSetting("FONT_DEFAULT")); // bylo 12.f
	mStatementArea->SetBackgroundImage( "data/cutscene/bg.png" );
	mStatementArea->SetVisible( false );

	mOptionsArea = gGUI.CreateWindow( "dialog-options", true, Z_GUI2 );
	mOptionsArea->SetPosition( 5.0f, 60.0f, 90.0f, 35.0f );
	mOptionsArea->SetPadding( 20.0f, 20.0f, 20.0f, 20.0f, UNIT_PIXEL );
	mOptionsArea->SetBackgroundImage( "data/cutscene/bg.png" );
	mOptionsArea->SetVisible( false );

	gGame.AddFrameListener( this );
	gGame.AddKeyListener( this );
}

CDialogDisplayer::~CDialogDisplayer()
{
	ExitDialog();
}


const std::wstring CDialogDisplayer::ParametrizeText(const std::wstring& text, const std::vector<std::string>& parameters)
{
    std::vector<std::wstring> replacements;

    for (size_t i = 0; i < parameters.size(); ++i)
    {
        // np. PLAYER0_BIND_Abi0
        if (parameters[i].substr(0, 13) == "PLAYER0_BIND_")
        {
            System::Input::CBindManager* bindMgr = System::Input::CBindManager::GetActualBindManager(0);

            // wyciag nazwe aktualnie zbindowanego klawisza i zamien ja na wstring
            std::string actionString = parameters[i].substr(13);
            if (bindMgr->GetKeyBindings().find(actionString) != bindMgr->GetKeyBindings().end())
                replacements.push_back(
                    StringUtils::ConvertToWString(
                        KeyStrings::KeyToString(
                            bindMgr->GetKeyBindings().find(parameters[i].substr(13))->second
                        )
                    )
                );
            else
                fprintf(stderr, "ERROR: invalid bind name: %s\n", actionString.c_str());
        }
        else if (parameters[i].substr(0, 13) == "PLAYER1_BIND_")
        {
            System::Input::CBindManager* bindMgr = System::Input::CBindManager::GetActualBindManager(1);

            // wyciag nazwe aktualnie zbindowanego klawisza
            std::string actionString = parameters[i].substr(13);
            if (bindMgr->GetKeyBindings().find(actionString) != bindMgr->GetKeyBindings().end())
                replacements.push_back(
                    StringUtils::ConvertToWString(
                        KeyStrings::KeyToString(
                            bindMgr->GetKeyBindings().find(parameters[i].substr(13))->second
                        )
                    )
                );
        }
        else
            fprintf(stderr, "ERROR: invalid cutscene text parameter: %s\n", parameters[i].c_str());
    }
    
    size_t max = (replacements.size() < 9 ? replacements.size() : 9);
    std::wstring ret = text;
    
    if (replacements.size() >= 9)
        fprintf(stderr, "WARNING: too many cutscene text parameters (max 9, found %d), replacing only first 9...\n", (int)parameters.size());

    for (size_t i = 0; i < max; ++i)
    {
        size_t at = 0;
        // po ludzku: znajduj wszystkie %1, %2, %3 ... %9
        while ((at = ret.find(L"%" + StringUtils::ToWString(i + 1), at)) != std::wstring::npos)
            ret.replace(at, 2, replacements[i].c_str(), replacements[i].size());
    }

    return ret;
}

void CDialogDisplayer::DisplayDialog( CDialogGraph *dialog )
{
	ExitDialog();

	if ( dialog != NULL )
	{
		mState = GRAPH_SEARCHING;
		mCurrentDialog = dialog;
		mCurrentDialog->LoadDialogs();
		SetCurrentNode( mCurrentDialog->GetStartNode() );
		gGame.SetFreezed( true );
	}
}

void CDialogDisplayer::ExitDialog()
{
	if ( mCurrentDialog != NULL )
	{
		mCurrentDialog->UnloadDialogs();
		mCurrentDialog = NULL;
		SetCurrentNode( NULL );
	}

	if ( mOptionTexts.size() > 0 )
	{
		mOptionTexts[0]->Remove();
		mOptionTexts.clear();
		mOptionNodes.clear();
	}

    if (mOptionsArea)
        mOptionsArea->SetVisible(false);
    if (mStatementArea)
        mStatementArea->SetVisible(false);

	mState = GRAPH_EMPTY;
	gGame.SetFreezed( false );
}

void CDialogDisplayer::KeyPressed(const sf::Event::KeyEvent &e)
{
	if ( mState == OPTION_CHOICE )
	{
		switch ( e.Code )
		{
		case sf::Key::Up:		SelectOption( mSelectedOption - 1 ); break;
		case sf::Key::Down:		SelectOption( mSelectedOption + 1 ); break;
        case sf::Key::Space:
		case sf::Key::Return:	ChooseSelectedOption(); return;
		default:				break;
		}
	}
	else if ( mState == STATEMENT_DISPLAY )
	{
		switch ( e.Code )
		{
            //dex: tu zmienilem
		//case sf::Key::Escape:
        case sf::Key::Space:    HideStatement(); break;
		default:				break;
		}
	}
}

void CDialogDisplayer::FrameStarted(float secondsPassed)
{
	if ( mState == GRAPH_SEARCHING )
	{
		vector<SDialogNode*> optionNodes;
		SDialogNode *		 statementNode = NULL;
		SDialogNode *		 otherNode = NULL;

		for ( unsigned i = 0; i < mCurrentNode->mChilds.size(); i++ )
		{
			SDialogNode * child = mCurrentDialog->GetNodeById( mCurrentNode->mChilds[i] );

			if ( IsNodeAvailable( child ) )
			{
				if		( child->mType == "option" )	optionNodes.push_back( child );
				else if	( child->mType == "player"
					   || child->mType == "npc" )		statementNode = child;
				else									otherNode = child;
			}
		}

		if ( optionNodes.size() > 0 )		DisplayOptions( optionNodes );
		else if ( statementNode != NULL )	DisplayStatement( statementNode );
		else if ( otherNode != NULL )		SetCurrentNode( otherNode );
		else								ExitDialog();
	}
}


/* PRIVATE METHODS: --------------------------------------------*/

void CDialogDisplayer::DisplayStatement(SDialogNode *statement)
{
    // dex: wylaczone
	//float readingSpeed = 10.0f;
	//mTimeToHideStatement = (float) statement->mString.size() / readingSpeed;

	mState = STATEMENT_DISPLAY;

	mStatementArea->SetText( ParametrizeText( statement->mString, statement->mStringParams ) );
	mStatementArea->SetVisible( true );
	SetCurrentNode( statement );
}

void CDialogDisplayer::HideStatement()
{
	mState = GRAPH_SEARCHING;
	mStatementArea->SetVisible( false );
}

void CDialogDisplayer::DisplayOptions( const std::vector<SDialogNode*>& options )
{
	for ( unsigned i = 0; i < options.size(); i++ )
	{
		CTextArea * optionText;

		if ( i == 0 )
		{
			optionText = mOptionsArea->CreateTextArea( "dialog_option", true, Z_GUI2-1 );
			optionText->SetPosition( 0.0f, 0.0f, 100.0f, 0.0f );
		}
		else
		{
			optionText = mOptionTexts[i-1]->CreateTextArea( "dialog_option", true, Z_GUI2-1 );
			optionText->SetPosition( 0.0f, 100.0f, 100.0f, 0.0f );
		}

		optionText->SetAutoHeight( true );
		optionText->SetText( options[i]->mString );
		optionText->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));

		mOptionTexts.push_back( optionText );
		mOptionNodes.push_back( options[i] );
	}

	mState = OPTION_CHOICE;
	mSelectedOption = 0;
	mOptionsArea->SetVisible( true );
	mOptionTexts[0]->SetColor( sf::Color::Green );
}

void CDialogDisplayer::SelectOption(unsigned int selected)
{
	unsigned int oldSelectedOption = mSelectedOption;
	mSelectedOption = selected % mOptionTexts.size();

	mOptionTexts[ oldSelectedOption ]->SetColor( sf::Color::White );
	mOptionTexts[ mSelectedOption ]->SetColor( sf::Color::Green );
}

void CDialogDisplayer::ChooseSelectedOption()
{
	DisplayStatement( mOptionNodes[ mSelectedOption ] );

	mOptionsArea->SetVisible( false );
	mOptionTexts[0]->Remove();
	mOptionTexts.clear();
	mOptionNodes.clear();
}

void CDialogDisplayer::SetCurrentNode(SDialogNode* node)
{
	if ( mCurrentNode == node )		return;
	if ( !(mCurrentNode = node) )	return;

	if ( mCurrentNode->mUniqueId != L"" )
	{
		gQuestManager.HandleEvent( new Quests::CStatementDisplay( mCurrentNode->mUniqueId ) );
	}

	/*	Przekazywanie przedmiotu w trakcie rozmowy:
	*	- nastepuje jako dodatek do normalnej wypowiedzi
	*	- zakladamy ze npc zawsze ma przedmiot ktory chce dac
	*	- jesli gracz nie ma przedmiotu ktory chce dac to caly wezel z wypowiedzia
	*	  jest tymczasowo 'niewidzialny' [ patrz metoda: IsNodeAvailable ]
	*/

	if ( mCurrentNode->mItemId != L"" )
	{
		if ( mCurrentNode->mType == "npc" )
		{
			gEquipment.AddItem( mCurrentNode->mItemId, mCurrentNode->mItemName );
			gQuestManager.HandleEvent( new Quests::CItemGiven( mCurrentNode->mItemId ) );
		}
		else
		{
			gEquipment.RemoveItem( mCurrentNode->mItemId );
			gQuestManager.HandleEvent( new Quests::CItemTaken( mCurrentNode->mItemId ) );
		}
	}
}

bool CDialogDisplayer::IsNodeAvailable( SDialogNode* node )
{
	return	( node->mRequiredFlag == L""	||  gQuestManager.FlagDefined( node->mRequiredFlag ) ) &&
			( node->mForbiddenFlag == L""	|| !gQuestManager.FlagDefined( node->mForbiddenFlag ) ) &&
			( node->mType == "npc" || node->mItemId == L"" || gEquipment.ContainsItem( node->mItemId ) );
}

