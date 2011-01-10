#include "CDialogGraph.h"
#include "../Utils/CXml.h"
#include "../Utils/StringUtils.h"
#include "../GUI/Localization/CLocalizator.h"

using namespace StringUtils;

CDialogGraph::CDialogGraph()
{
	mStartNode.mType = "start";
}

CDialogGraph::~CDialogGraph()
{
	UnloadDialogs();
	mDialogFiles.clear();
}

void CDialogGraph::LoadDialogs()
{
	UnloadDialogs();

	for ( unsigned i = 0; i < mDialogFiles.size(); i++ )
	{
		LoadDialog( mDialogFiles[i] );
	}
}

void CDialogGraph::UnloadDialogs()
{
	mDialogNodes.clear();
	mStartNode.mChilds.clear();
}

void CDialogGraph::AddNode(const SDialogNode &node, unsigned int id)
{
	if ( mDialogNodes.size() <= id )
	{
		mDialogNodes.resize( id + 1 );
	}

	mDialogNodes[id] = node;
}

void CDialogGraph::LoadDialog(const std::string &file)
{
	CXml xml( file, "root" );

	if ( xml.GetString( NULL, "type" ) == "dialog" )
	{
		unsigned nodeIdInc = (unsigned) mDialogNodes.size();

		for ( xml_node<>* node = xml.GetChild( NULL, "startNode" ); node; node = xml.GetSibl( node, "startNode" ) )
		{
			mStartNode.mChilds.push_back( (unsigned) xml.GetInt( node ) + nodeIdInc ); 
		}

		for ( xml_node<>* node = xml.GetChild( NULL, "node" ); node; node = xml.GetSibl( node, "node" ) )
		{
			unsigned int nodeId;
			SDialogNode dialogNode;

			xml_node<>* nodeType =			xml.GetChild( node, "type" );
			xml_node<>* nodeUniqueId =		xml.GetChild( node, "uniqueId" );
			xml_node<>* nodeRequiredFlag =	xml.GetChild( node, "requiredFlag" );
			xml_node<>* nodeForbiddenFlag = xml.GetChild( node, "forbiddenFlag" );
			xml_node<>* nodeItemId =		xml.GetChild( node, "itemId" );
			xml_node<>* nodeItemName =		xml.GetChild( node, "itemName" );

			nodeId = (unsigned)			xml.GetInt( xml.GetChild( node, "id" ) ) + nodeIdInc;
			dialogNode.mString =		gLocalizator.Localize( xml.GetString( xml.GetChild( node, "string" ) ) );
            for (xml_node<>* subNode = xml.GetChild(node, "param"); subNode; subNode = xml.GetSibl(subNode, "param"))
                dialogNode.mStringParams.push_back(xml.GetString(subNode));

			dialogNode.mType =			nodeType ?			xml.GetString( nodeType ) : "";
			dialogNode.mUniqueId =		nodeUniqueId ?		ReinterpretFromUTF8( xml.GetString( nodeUniqueId ) )		: L"";
			dialogNode.mRequiredFlag =	nodeRequiredFlag ?	ReinterpretFromUTF8( xml.GetString( nodeRequiredFlag ) )	: L"";
			dialogNode.mForbiddenFlag =	nodeForbiddenFlag ?	ReinterpretFromUTF8( xml.GetString( nodeForbiddenFlag ) )	: L"";
			dialogNode.mItemId =		nodeItemId ?		ReinterpretFromUTF8( xml.GetString( nodeItemId ) )			: L"";
			dialogNode.mItemName =		nodeItemName ?		ReinterpretFromUTF8( xml.GetString( nodeItemName ) )		: L"";

			for ( xml_node<>* child = xml.GetChild( node, "child" ); child; child = xml.GetSibl( child, "child" ) )
			{
				dialogNode.mChilds.push_back( (unsigned) xml.GetInt( child ) + nodeIdInc );
			}

			AddNode( dialogNode, nodeId );
		}
	}
}

