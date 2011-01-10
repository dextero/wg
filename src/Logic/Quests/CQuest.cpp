#include "CQuest.h"
#include "CQuestEvent.h"
#include "../../Commands/CCommands.h"
#include "../../Utils/CXml.h"
#include "../../Utils/StringUtils.h"
#include "../../GUI/Notepad/CNotepad.h"
#include "../../GUI/Localization/CLocalizator.h"
#include <assert.h>

using namespace Quests;
using namespace StringUtils;
using namespace std;

CQuest::CQuest()
{
	mCurrentNode = &mStartNode;
}

CQuest::~CQuest()
{
}

void CQuest::Load(const std::string &file)
{
	CXml xml( file, "root" );	

	if ( xml.GetString( NULL, "type" ) == "quest" )
	{			
		for ( xml_node<>* node = xml.GetChild( NULL, "startNode" ); node; node = xml.GetSibl( node, "startNode" ) )
			mStartNode.mChilds.push_back( (unsigned) xml.GetInt( node ) );

		for ( xml_node<>* node = xml.GetChild( NULL, "globalNode" ); node; node = xml.GetSibl( node, "globalNode" ) )
			mGlobalNodes.push_back( (unsigned) xml.GetInt( node ) );

		for ( xml_node<>* node = xml.GetChild( NULL, "node" ); node; node = xml.GetSibl( node, "node" ) )
		{
			SQuestNode questNode;

			/* requirements */

			for ( xml_node<>* req = xml.GetChild( node, "req" ); req; req = xml.GetSibl( req, "req" ) )
			{
				string type = xml.GetString( xml.GetChild( req, "type" ) );
				vector<wstring> params;

				for ( xml_node<>* param = xml.GetChild( req, "param" ); param; param = xml.GetSibl( param, "param" ) )
					params.push_back( ReinterpretFromUTF8( xml.GetString( param ) ) );

				AddRequirement( type, params, questNode );
			}

			/* actions */

			for ( xml_node<>* action = xml.GetChild( node, "action" ); action; action = xml.GetSibl( action, "action" ) )
				questNode.mActions.push_back( ReinterpretFromUTF8( xml.GetString( action ) ) );

			/* notes */

			for ( xml_node<>* nt = xml.GetChild( node, "note" ); nt; nt = xml.GetSibl( nt, "note" ) )
			{
				SNote note;
				note.pageCaption =	gLocalizator.Localize( xml.GetString( xml.GetChild( nt, "page" ) ) );
				note.content =		gLocalizator.Localize( xml.GetString( xml.GetChild( nt, "content" ) ) );
				questNode.mNotes.push_back( note );
			}

			/* childs */

			for ( xml_node<>* child = xml.GetChild( node, "child" ); child; child = xml.GetSibl( child, "child" ) )
				questNode.mChilds.push_back( (unsigned) xml.GetInt( child ) );

			unsigned nodeId = xml.GetInt( xml.GetChild( node, "id" ) );
			AddNode( questNode, nodeId );
		}
	}
}

void CQuest::Update(Quests::CQuestEvent *e)
{
	assert( mCurrentNode );

	for ( unsigned i = 0; i < mCurrentNode->mChilds.size(); i++ )
	{
		unsigned childId = mCurrentNode->mChilds[i];

		if ( CheckNodeRequirements( childId, e ) )
		{
			ExecuteNode( childId );
			mCurrentNode = &mQuestNodes[ childId ];
			Update( e );
			return;
		}
	}

	for ( unsigned i = 0; i < mGlobalNodes.size(); i++ )
	{
		if ( CheckNodeRequirements( mGlobalNodes[i], e ) )
		ExecuteNode( mGlobalNodes[i] );
	}
}

/* ========================== PRIVATE METHODS ========================== */

bool CQuest::CheckNodeRequirements(unsigned int nodeId, CQuestEvent* e)
{
	SQuestNode & node = mQuestNodes[ nodeId ];
	bool nodeAchieved = true;

	for ( unsigned j = 0; j < node.mRequirements.size(); j++ )
	{
		node.mRequirements[j]->Update( e );
		nodeAchieved = nodeAchieved && node.mRequirements[j]->Achieved();
	}

	return nodeAchieved;
}

void CQuest::ExecuteNode( unsigned nodeId )
{
	SQuestNode & node = mQuestNodes[ nodeId ];

	for ( unsigned j = 0; j < node.mActions.size(); j++ )	gCommands.ParseCommand( node.mActions[ j ] );
	for ( unsigned j = 0; j < node.mNotes.size(); j++ )		gNotepad.AddNote( node.mNotes[j].pageCaption, node.mNotes[j].content );
}

void CQuest::AddNode(const SQuestNode &node, unsigned int id)
{
	if ( mQuestNodes.size() <= id )
	{
		mQuestNodes.resize( id + 1 );
	}

	mQuestNodes[ id ] = node;
}

void CQuest::AddRequirement(std::string &type, std::vector<std::wstring> &params, Quests::SQuestNode &destNode)
{
	CQuestNodeRequirement* requirement = new CQuestNodeRequirement;

	if ( type == "PhysicalDeathById" )	
	{
		unsigned count = params.size() >= 2 ? Parse<unsigned>( params[1] ) : 1;
		requirement->SetRequiredEvent( new CPhysicalDeathById( params[0] ), count );
	}
	else if ( type == "PhysicalDeathByGenre" )
	{
		unsigned count = params.size() >= 2 ? Parse<unsigned>( params[1] ) : 1;
		requirement->SetRequiredEvent( new CPhysicalDeathByGenre( params[0] ), count );
	}
	else if ( type == "StatementDisplay" )
	{
		unsigned count = params.size() >= 2 ? Parse<unsigned>( params[1] ) : 1;
		requirement->SetRequiredEvent( new CStatementDisplay( params[0] ), count );
	}
	else if ( type == "ItemGiven" )
	{
		unsigned count = params.size() >= 2 ? Parse<unsigned>( params[1] ) : 1;
		requirement->SetRequiredEvent( new CItemGiven( params[0] ), count );
	}
	else if ( type == "ItemTaken" )
	{
		unsigned count = params.size() >= 2 ? Parse<unsigned>( params[1] ) : 1;
		requirement->SetRequiredEvent( new CItemTaken( params[0] ), count );
	}
	else if ( type == "FlagDefined" )
	{
		requirement->SetRequiredFlag( params[0] );
	}
    else if ( type == "DifficultySet" )
    {
        unsigned count = params.size() >= 2 ? Parse<unsigned>( params[1] ) : 1;
        requirement->SetRequiredEvent( new CDifficultySet( params[0] ), count );
    }
	
	destNode.mRequirements.push_back( requirement );
}

