#ifndef __CQUADTREENODE_H__
#define __CQUADTREENODE_H__

#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include "CSceneNode.h"


class CQuadTreeSceneNode{
public:

	CQuadTreeSceneNode();
	~CQuadTreeSceneNode();

	inline void SetNodeId(int id)				{ m_nodeId = id; }
	inline int GetNodeId()						{ return m_nodeId; }

	inline void SetParentNodeId(int parentId)	{ m_parentNodeId = parentId; }
	inline int GetParentNodeId()				{ return m_parentNodeId; }

	inline void SetHasChilds(bool hasChilds)	{ m_hasChilds = hasChilds; }
	inline bool GetHasChilds()					{ return m_hasChilds; }

	void SetVisibility(bool visibility);
	bool GetVisibility();

	void SetBoundingRect(float left, float top, float right, float bottom);
	sf::FloatRect GetBoundingRect();

	void Add(CSceneNode* node);
	bool Remove(CSceneNode* node, bool eraseContent);

	inline std::vector<CSceneNode*> * GetMovingPhysicals()		{ return &m_movingPhysicals; }
	inline std::vector<CSceneNode*> * GetNonMovingPhysicals()	{ return &m_nonMovingPhysicals; }
	inline std::vector<CSceneNode*> * GetDisplayables()			{ return &m_displayables; }

	inline std::vector<CSceneNode*> & GetMovingPhysicalsR()		{ return m_movingPhysicals; }
	inline std::vector<CSceneNode*> & GetNonMovingPhysicalsR()	{ return m_nonMovingPhysicals; }
	inline std::vector<CSceneNode*> & GetDisplayablesR()		{ return m_displayables; }

	bool Contains(CSceneNode * node);
	int GetNodesCount();

	void ReleaseResources();

private:
	int m_nodeId;
	int m_parentNodeId;
	bool m_hasChilds;
	bool m_visibility;
	sf::FloatRect m_boundingRect;

	std::vector<CSceneNode*> m_movingPhysicals;
	std::vector<CSceneNode*> m_nonMovingPhysicals;
	std::vector<CSceneNode*> m_displayables;
};

#endif
