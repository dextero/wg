#include"CQuadTreeNode.h"
#include"../../Utils/SafeFunctions.h"
#include"../CMap.h"
#include"../../Logic/CPhysical.h"
#include"../../Logic/CPhysicalManager.h"
#include"../../Rendering/CDrawableManager.h"
#include"../../Rendering/CDisplayable.h"
#include<iostream>

CQuadTreeSceneNode::CQuadTreeSceneNode():
    m_nodeId(0),
    m_parentNodeId(0),
    m_hasChilds(false),
    m_visibility(false)
{
	ReleaseResources();
}

CQuadTreeSceneNode::~CQuadTreeSceneNode(){
	ReleaseResources();
}

void CQuadTreeSceneNode::ReleaseResources(){

	for ( unsigned i = 0; i < m_movingPhysicals.size(); i++ )
		if ( m_movingPhysicals[i]->GetPhysical() != NULL && m_movingPhysicals[i]->GetPhysical()->GetCategory() != PHYSICAL_PLAYER ) {
			m_movingPhysicals[i]->SetParentNodeId( -1 );
			m_movingPhysicals[i]->GetPhysical()->MarkForDelete();
		}

	for ( unsigned i = 0; i < m_nonMovingPhysicals.size(); i++ )
		if ( m_nonMovingPhysicals[i]->GetPhysical() != NULL ) {
			m_nonMovingPhysicals[i]->SetParentNodeId( -1 );
			m_nonMovingPhysicals[i]->GetPhysical()->MarkForDelete();
		}

	for ( unsigned i = 0; i < m_displayables.size(); i++ ) {
		if ( m_displayables[i]->GetDisplayable() != NULL )
		gDrawableManager.DestroyDrawable( (IDrawable*) m_displayables[i]->GetDisplayable() );
		delete m_displayables[i];
	}

	m_movingPhysicals.clear();
	m_nonMovingPhysicals.clear();
	m_displayables.clear();
}

void  CQuadTreeSceneNode::SetVisibility(bool visibility){

    m_visibility = visibility;

	for ( unsigned i = 0; i < m_movingPhysicals.size(); i++ )
		m_movingPhysicals[i]->SetVisibility( visibility );

	for ( unsigned i = 0; i < m_nonMovingPhysicals.size(); i++ )
		m_nonMovingPhysicals[i]->SetVisibility( visibility );

	for ( unsigned i = 0; i < m_displayables.size(); i++ )
		m_displayables[i]->SetVisibility( visibility );
}

bool CQuadTreeSceneNode::GetVisibility() {
	return m_visibility;
}

void CQuadTreeSceneNode::SetBoundingRect(float left, float top, float right, float bottom) {

	m_boundingRect.Left = left;
	m_boundingRect.Top = top;
	m_boundingRect.Right = right;
	m_boundingRect.Bottom = bottom;
}

sf::FloatRect CQuadTreeSceneNode::GetBoundingRect() {
	return m_boundingRect;
}

void CQuadTreeSceneNode::Add(CSceneNode * node) {

	node->SetParentNodeId( m_nodeId );

	if ( node->GetPhysical() != NULL && (node->GetPhysical()->GetCategory() & PHYSICAL_MOVING) != 0 )
		m_movingPhysicals.push_back( node );
	else if ( node->GetPhysical() != NULL )
		m_nonMovingPhysicals.push_back( node );
	else
		m_displayables.push_back( node );
}

bool CQuadTreeSceneNode::Remove(CSceneNode *node, bool eraseContent) {

	std::vector<CSceneNode*> * nodes;
	std::vector<CSceneNode*>::iterator it;

	nodes = &m_movingPhysicals;
	for ( it = nodes->begin(); it != nodes->end(); ++it )
	{
		if ( (*it) == node ) 
		{
			if ( eraseContent )
				node->GetPhysical()->MarkForDelete();
			node->SetParentNodeId( -1 );
			nodes->erase( it );
			return true;
		}
	}

	nodes = &m_nonMovingPhysicals;
	for ( it = nodes->begin(); it != nodes->end(); ++it )
	{
		if ( (*it) == node ) 
		{
			if ( eraseContent )
				node->GetPhysical()->MarkForDelete();
			node->SetParentNodeId( -1 );
			nodes->erase( it );
			return true;
		}
	}

	nodes = &m_displayables;
	for ( it = nodes->begin(); it != nodes->end(); ++it )
	{
		if ( (*it) == node ) 
		{
			if ( eraseContent ) {
				gDrawableManager.DestroyDrawable( node->GetDisplayable() );
				delete node;
			}
			else node->SetParentNodeId( -1 );
			nodes->erase( it );
			return true;
		}
	}

	return false;
}

bool CQuadTreeSceneNode::Contains(CSceneNode* node) {

	sf::FloatRect nodeRect = node->GetBoundingRect();

	sf::Vector2f thisCenter;
	thisCenter.x = (m_boundingRect.Left + m_boundingRect.Right) * 0.5f;
	thisCenter.y = (m_boundingRect.Top + m_boundingRect.Bottom) * 0.5f;

	return	nodeRect.Left >= m_boundingRect.Left && nodeRect.Right <= m_boundingRect.Right &&
			nodeRect.Top >= m_boundingRect.Top && nodeRect.Bottom <= m_boundingRect.Bottom && 
			( (!m_hasChilds ) || 
			  ( nodeRect.Left <= thisCenter.x && nodeRect.Right >= thisCenter.x ) ||
			  ( nodeRect.Top <= thisCenter.y && nodeRect.Bottom >= thisCenter.y ) );
			
}

int CQuadTreeSceneNode::GetNodesCount() {
	return (int) (m_movingPhysicals.size() + m_nonMovingPhysicals.size() + m_displayables.size());
}
