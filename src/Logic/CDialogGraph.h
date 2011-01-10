#ifndef _DIALOG_TREE_
#define _DIALOG_TREE_

#include <string>
#include <vector>

struct SDialogNode
{
	std::string mType;
	std::wstring mUniqueId;
	std::wstring mRequiredFlag;
	std::wstring mForbiddenFlag;
	std::wstring mItemId;
	std::wstring mItemName;
	std::wstring mString;
    std::vector<std::string> mStringParams;
	std::vector<unsigned int> mChilds;

	~SDialogNode() {}
};

class CDialogGraph
{
public:
	CDialogGraph();
	~CDialogGraph();

	void LoadDialogs();
	void UnloadDialogs();

	inline void AddDialogFile( const std::string& file )	{ mDialogFiles.push_back( file ); }
	inline SDialogNode* GetStartNode()						{ return &mStartNode; }
	inline SDialogNode* GetNodeById( unsigned int id )		{ return ( id < mDialogNodes.size() ? &mDialogNodes[id] : NULL ); }

private:
	void AddNode( const SDialogNode& node, unsigned int id );
	void LoadDialog( const std::string& file );

	SDialogNode mStartNode;
	std::vector<SDialogNode> mDialogNodes;
	std::vector<std::string> mDialogFiles;
};

#endif

