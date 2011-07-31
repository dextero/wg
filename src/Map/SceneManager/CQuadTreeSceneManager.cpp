#include"CQuadTreeSceneManager.h"
#include"../../Utils/SafeFunctions.h"
#include"../../Utils/Maths.h"
#include"../../Logic/CPhysical.h"
#include"../../Logic/CPhysicalManager.h"
#include"../CMap.h"
#include <vector>
#include <string.h>

#define IS_LEFT_TOP_CHILD(x,y)		( (x) == ( (x)>>1<<1 )	 && (y) == ( (y)>>1<<1 ) )
#define IS_RIGHT_TOP_CHILD(x,y)		( (x) == ( (x)>>1<<1 )+1 && (y) == ( (y)>>1<<1 ) )
#define IS_LEFT_BOTTOM_CHILD(x,y)	( (x) == ( (x)>>1<<1 )   && (y) == ( (y)>>1<<1 )+1 )
#define IS_RIGHT_BOTTOM_CHILD(x,y)	( (x) == ( (x)>>1<<1 )+1 && (y) == ( (y)>>1<<1 )+1 )

// Public functions 

CQuadTreeSceneManager::CQuadTreeSceneManager(){
    memset(m_physicalCount, 0, 32 * sizeof(int));
	DropResources();
}

CQuadTreeSceneManager::~CQuadTreeSceneManager(){
	DropResources();
}

void CQuadTreeSceneManager::Initialize(sf::Vector2<int> size){
	DropResources();
	m_size = size;
	m_quadTreeScale.x = (float) ( 1 << (LevelsCount-1) ) / m_size.x;	// 2^(LevelsCount-1) / m_size.x
	m_quadTreeScale.y = (float) ( 1 << (LevelsCount-1) ) / m_size.y;
	m_quadTreeNodes.resize( ( (1<<2*LevelsCount) - 1 ) / 3 );			// (4^LevelsCount - 1)/(4-1)
	BuildLogicalStructure();
}
	
void CQuadTreeSceneManager::AddSceneNode(CSceneNode * node){

	int qtNodeId = GetNodeIdContaining( node );
	
	if ( qtNodeId >= 0 && qtNodeId < (int) m_quadTreeNodes.size() )
	{
		m_quadTreeNodes[qtNodeId].Add( node );
		++ m_totalAddedObjectsCount;
		++ m_totalObjectsCount;
	}
}

void CQuadTreeSceneManager::RemoveSceneNode(CSceneNode * node, bool eraseContent){

	int qtNodeId = node->GetParentNodeId();

	if ( qtNodeId >= 0 && qtNodeId < (int) m_quadTreeNodes.size() )
		if ( m_quadTreeNodes[qtNodeId].Remove( node, eraseContent ) )
			-- m_totalObjectsCount;
}

void CQuadTreeSceneManager::ClearScene(){
	DropResources();
	gPhysicalManager.FrameStarted( 0.0f ); // usun physicale zaznaczone do usuniecia
}

void CQuadTreeSceneManager::UpdateScene(){
	std::vector<CSceneNode*> * nodes;
	
	for ( unsigned i = 0; i < m_quadTreeNodes.size(); i++ )
	{
		nodes = m_quadTreeNodes[i].GetMovingPhysicals();
		for ( unsigned j = 0; j < nodes->size(); j++ )
		{
			CSceneNode * currentNode = (*nodes)[j];

			if ( !m_quadTreeNodes[i].Contains( currentNode ) )
			{
				int newQtNodeId = GetNodeIdContaining( currentNode );

				if ( newQtNodeId != (int)i ) {
					m_quadTreeNodes[newQtNodeId].Add( currentNode );
					nodes->erase( nodes->begin() + j );
					--j;
				}
			}
		}

		nodes = m_quadTreeNodes[i].GetNonMovingPhysicals();
		for ( unsigned j = 0; j < nodes->size(); j++ )
		{
			CSceneNode * currentNode = (*nodes)[j];

			if ( (currentNode->GetPhysical() && (currentNode->GetPhysical()->GetSideAndCategory().category & PHYSICAL_MOVING)) || (!m_quadTreeNodes[i].Contains(currentNode)) )
			{
				int newQtNodeId = GetNodeIdContaining( currentNode );

				if ( newQtNodeId != (int)i ) {
					m_quadTreeNodes[newQtNodeId].Add( currentNode );
					nodes->erase( nodes->begin() + j );
					--j;
				}
			}
		}
	}

	/* oblicz liczbe physicali */
	for ( unsigned i = 0; i < 32; i++ )
		m_physicalCount[i] = 0;

	for ( unsigned i = 0; i < m_quadTreeNodes.size(); i++ )
	{
		nodes = m_quadTreeNodes[i].GetMovingPhysicals();
		for ( unsigned j = 0; j < nodes->size(); j++ )
			if ( (*nodes)[j]->GetPhysical() )
				++m_physicalCount[ GetPhysicalCategoryId( (*nodes)[j]->GetPhysical()->GetSideAndCategory().category ) ];

		nodes = m_quadTreeNodes[i].GetNonMovingPhysicals();
		for ( unsigned j = 0; j < nodes->size(); j++ )
			if ( (*nodes)[j]->GetPhysical() )
				++m_physicalCount[ GetPhysicalCategoryId( (*nodes)[j]->GetPhysical()->GetSideAndCategory().category ) ];
	}
}

void CQuadTreeSceneManager::CullVisibility( const sf::FloatRect & viewRectangle ){

	if ( m_quadTreeNodes.size() > 0 )
	{
		m_quadTreeNodes[0].SetVisibility(true);
		m_currentDrawObjectsCount = m_quadTreeNodes[0].GetNodesCount();
	}

	for ( unsigned i = 1; i < m_quadTreeNodes.size(); i++ )
	{
		if ( !m_quadTreeNodes[ m_quadTreeNodes[i].GetParentNodeId() ].GetVisibility() )
		{
			m_quadTreeNodes[i].SetVisibility(false);
		}
		else if ( viewRectangle.Intersects( m_quadTreeNodes[i].GetBoundingRect() ) ) 
		{
			m_quadTreeNodes[i].SetVisibility(true);
			m_currentDrawObjectsCount += m_quadTreeNodes[i].GetNodesCount();
		}
		else
		{
			m_quadTreeNodes[i].SetVisibility(false);
		}
	}
}

void CQuadTreeSceneManager::SceneStats( int & currentDrawObjectsCount, int & totalObjectsCount){

	currentDrawObjectsCount = m_currentDrawObjectsCount;
	totalObjectsCount = m_totalObjectsCount;
}

void CQuadTreeSceneManager::GetPhysicalsInRadius(const sf::Vector2f& pos, float radius, int category, ESide side, std::vector<CPhysical*>& outPhysicals)
{
	outPhysicals.clear();

	if ( m_quadTreeNodes.size() > 0 )
	{
		unsigned x1 = Maths::ClampInt( unsigned((pos.x-radius) * m_quadTreeScale.x), 0, (1<<(LevelsCount-1))-1 );
		unsigned y1 = Maths::ClampInt( unsigned((pos.y-radius) * m_quadTreeScale.y), 0, (1<<(LevelsCount-1))-1 );
		unsigned x2 = Maths::ClampInt( unsigned((pos.x+radius) * m_quadTreeScale.x), 0, (1<<(LevelsCount-1))-1 );
		unsigned y2 = Maths::ClampInt( unsigned((pos.y+radius) * m_quadTreeScale.y), 0, (1<<(LevelsCount-1))-1 );
		int level = LevelsCount - 1;
		CPhysical * physical;

		while ( level >= 0 )
		{
			for ( unsigned y = y1; y <= y2; y++ )
			for ( unsigned x = x1; x <= x2; x++ )
			{
				unsigned nodeId = GetNodeId( x, y, level );
				
				if ( category & PHYSICAL_MOVING )
				for ( unsigned i = 0; i < m_quadTreeNodes[ nodeId ].GetMovingPhysicalsR().size(); i++ )
				{
					physical = m_quadTreeNodes[ nodeId ].GetMovingPhysicalsR()[ i ]->GetPhysical();
					
					physCategory const victimCategory = ReinterpretCategoryForVersusMode(side, physical->GetSideAndCategory().side, physical->GetSideAndCategory().category);
					if (( victimCategory  & category ) && 
						( Maths::Length( physical->GetPosition() - pos ) < physical->GetCircleRadius() + radius ) )
					{
						outPhysicals.push_back( physical );
					}
				}

				if ( category & PHYSICAL_NONMOVING )
				for ( unsigned i = 0; i < m_quadTreeNodes[ nodeId ].GetNonMovingPhysicalsR().size(); i++ )
				{
					physical = m_quadTreeNodes[ nodeId ].GetNonMovingPhysicalsR()[ i ]->GetPhysical();
					
					if (( physical->GetSideAndCategory().category & category ) &&
						( Maths::Length( physical->GetPosition() - pos ) < physical->GetCircleRadius() + radius ) )
					{
						outPhysicals.push_back( physical );
					}
				}
			}

			x1 >>= 1;
			x2 >>= 1;
			y1 >>= 1;
			y2 >>= 1;
			--level;
		}
	}
}

void CQuadTreeSceneManager::GetPhysicalsBetweenPoints(const sf::Vector2f& a, const sf::Vector2f& b, float tolerance, 
													  int category, std::vector<CPhysical*>& outPhysicals)
{
	outPhysicals.clear();

	if ( m_quadTreeNodes.size() > 0 )
	{
		unsigned x1 = Maths::ClampInt( unsigned(std::min(a.x,b.x) * m_quadTreeScale.x), 0, (1<<(LevelsCount-1))-1 );
		unsigned y1 = Maths::ClampInt( unsigned(std::min(a.y,b.y) * m_quadTreeScale.y), 0, (1<<(LevelsCount-1))-1 );
		unsigned x2 = Maths::ClampInt( unsigned(std::max(a.x,b.x) * m_quadTreeScale.x), 0, (1<<(LevelsCount-1))-1 );
		unsigned y2 = Maths::ClampInt( unsigned(std::max(a.y,b.y) * m_quadTreeScale.y), 0, (1<<(LevelsCount-1))-1 );
		int level = LevelsCount - 1;
		CPhysical * physical;

		while ( level >= 0 )
		{
			for ( unsigned y = y1; y <= y2; y++ )
			for ( unsigned x = x1; x <= x2; x++ )
			{
				unsigned nodeId = GetNodeId( x, y, level );
				
				if ( category & PHYSICAL_MOVING )
				for ( unsigned i = 0; i < m_quadTreeNodes[ nodeId ].GetMovingPhysicalsR().size(); i++ )
				{
					physical = m_quadTreeNodes[ nodeId ].GetMovingPhysicalsR()[ i ]->GetPhysical();

					if (( physical->GetSideAndCategory().category & category ) && 
						( Maths::PointToSegment(physical->GetPosition(), a, b) < physical->GetCircleRadius() + tolerance ))
					{
						outPhysicals.push_back( physical );
					}
				}

				if ( category & PHYSICAL_NONMOVING )
				for ( unsigned i = 0; i < m_quadTreeNodes[ nodeId ].GetNonMovingPhysicalsR().size(); i++ )
				{
					physical = m_quadTreeNodes[ nodeId ].GetNonMovingPhysicalsR()[ i ]->GetPhysical();
					
					if (( physical->GetSideAndCategory().category & category ) &&
						( Maths::PointToSegment(physical->GetPosition(), a, b) < physical->GetCircleRadius() + tolerance ))
					{
						outPhysicals.push_back( physical );
					}
				}
			}

			x1 >>= 1;
			x2 >>= 1;
			y1 >>= 1;
			y2 >>= 1;
			--level;
		}
	}
}

bool CQuadTreeSceneManager::ContainsPhysicals(int category) {

	int category_ = category;
	int categoryId = 0;

	while ( category_ > 0 )
	{
		categoryId = GetPhysicalCategoryId( category_ );
		if ( m_physicalCount[ categoryId ] > 0 )
			return true;
		category_ -= ( 1 << (categoryId-1) );
	}

	return false;
}

CSceneNode* CQuadTreeSceneManager::GetDisplayableAt(const sf::Vector2f& v) {

	CSceneNode * out = NULL;

	if ( m_quadTreeNodes.size() > 0 )
	{
		unsigned uX = Maths::ClampInt( unsigned(v.x * m_quadTreeScale.x), 0, (1<<(LevelsCount-1))-1 );
		unsigned uY = Maths::ClampInt( unsigned(v.y * m_quadTreeScale.y), 0, (1<<(LevelsCount-1))-1 );
		std::vector<CSceneNode*> * sceneNodes;

		for ( int level = LevelsCount-1; level >= 0; level-- )
		{
			sceneNodes = m_quadTreeNodes[ GetNodeId( uX, uY, level ) ].GetDisplayables();

			for ( unsigned i = sceneNodes->size() - 1; i < (unsigned)-1; --i )  // przepelnienie
				if ( sceneNodes->at( i )->GetBoundingRect().Contains( v.x, v.y ) )
				if ( out == NULL || Maths::Length(v-sceneNodes->at(i)->GetPosition()) < Maths::Length(v-out->GetPosition()) )
					out = sceneNodes->at( i );

			uX >>= 1;
			uY >>= 1;
		}
	}

	return out;
}

CSceneNode* CQuadTreeSceneManager::GetPhysicalAt(const sf::Vector2f& v) {

	if ( m_quadTreeNodes.size() > 0 )
	{
		unsigned uX = Maths::ClampInt( unsigned(v.x * m_quadTreeScale.x), 0, (1<<(LevelsCount-1))-1 );
		unsigned uY = Maths::ClampInt( unsigned(v.y * m_quadTreeScale.y), 0, (1<<(LevelsCount-1))-1 );
		std::vector<CSceneNode*> * sceneNodes;
		for ( int level = LevelsCount-1; level >= 0; level-- )
		{
			sceneNodes = m_quadTreeNodes[ GetNodeId( uX, uY, level ) ].GetMovingPhysicals();

			for ( unsigned i = sceneNodes->size() - 1; i < (unsigned)-1; --i )  // przepelnienie
				if ( sceneNodes->at( i )->GetBoundingRect().Contains( v.x, v.y ) )
                    return sceneNodes->at( i );

			sceneNodes = m_quadTreeNodes[ GetNodeId( uX, uY, level ) ].GetNonMovingPhysicals();

			for ( unsigned i = sceneNodes->size() - 1; i < (unsigned)-1; --i )  // przepelnienie
				if ( sceneNodes->at( i )->GetBoundingRect().Contains( v.x, v.y ) )
                    return sceneNodes->at( i );

			uX >>= 1;
			uY >>= 1;
		}
	}

	return NULL;
}

std::vector<CQuadTreeSceneNode> & CQuadTreeSceneManager::GetQuadTreeNodes() {
	return m_quadTreeNodes;
}

int CQuadTreeSceneManager::GetPhysicalCount(int type)
{
    int out = 0;
    for (int i = 0; i < PHYSICAL_CATEGORIES_COUNT; ++i)
    {
        if (type & (1 << i))
            out += m_physicalCount[i + 1];
    }

    return out;
}

sf::Vector2i CQuadTreeSceneManager::GetSize() {
	return m_size;
}

// ========================================================================================

// Private functions 

void CQuadTreeSceneManager::DropResources(){

	m_quadTreeNodes.clear();
	m_currentDrawObjectsCount = 0;
	m_totalObjectsCount = 0;
	m_totalAddedObjectsCount = 0;
}

void CQuadTreeSceneManager::BuildLogicalStructure(){

	m_quadTreeNodes[0].SetNodeId( 0 );
	m_quadTreeNodes[0].SetParentNodeId( -1 );
	m_quadTreeNodes[0].SetHasChilds( false );
	m_quadTreeNodes[0].SetBoundingRect( 0.0f, 0.0f, (float)m_size.x, (float)m_size.y );

	int x = 0, y = 0, level = 1;

	for ( unsigned i = 1; i < m_quadTreeNodes.size(); i++ )
	{
		if ( x >= (1<<level) )	{ x = 0; ++y; }
		if ( y >= (1<<level) )	{ x = y = 0; ++level; }

		int parentId = GetNodeId( x>>1, y>>1, level-1 );
		m_quadTreeNodes[i].SetNodeId( i );
		m_quadTreeNodes[i].SetParentNodeId( parentId );
		m_quadTreeNodes[i].SetHasChilds( false );
		m_quadTreeNodes[parentId].SetHasChilds( true );

		sf::FloatRect parentRect = m_quadTreeNodes[parentId].GetBoundingRect();
		if ( IS_LEFT_TOP_CHILD(x,y) ) 
			m_quadTreeNodes[i].SetBoundingRect
			(
				parentRect.Left,
				parentRect.Top,
				(parentRect.Left+parentRect.Right)*0.5f,
				(parentRect.Top+parentRect.Bottom)*0.5f
			);
		else if ( IS_RIGHT_TOP_CHILD(x,y) )
			m_quadTreeNodes[i].SetBoundingRect
			(
				(parentRect.Left+parentRect.Right)*0.5f,
				parentRect.Top,
				parentRect.Right,
				(parentRect.Top+parentRect.Bottom)*0.5f
			);
		else if ( IS_LEFT_BOTTOM_CHILD(x,y) )
			m_quadTreeNodes[i].SetBoundingRect
			(
				parentRect.Left,
				(parentRect.Top+parentRect.Bottom)*0.5f,
				(parentRect.Left+parentRect.Right)*0.5f,
				parentRect.Bottom
			);
		else if ( IS_RIGHT_BOTTOM_CHILD(x,y) )
			m_quadTreeNodes[i].SetBoundingRect
			(
				(parentRect.Left+parentRect.Right)*0.5f,
				(parentRect.Top+parentRect.Bottom)*0.5f,
				parentRect.Right,
				parentRect.Bottom
			);

		++x;
	}
}

int CQuadTreeSceneManager::GetNodeIdContaining(CSceneNode *node)
{
	sf::FloatRect nodeRect = node->GetBoundingRect();

	if ( nodeRect.Right > (float) m_size.x || nodeRect.Left < 0.0f ||
		 nodeRect.Bottom > (float) m_size.y || nodeRect.Top < 0.0f )		
		 return 0;

	int x = (int) ( nodeRect.Left * m_quadTreeScale.x );
	int y = (int) ( nodeRect.Top * m_quadTreeScale.y );

	int xResult = x ^ ((int) ( nodeRect.Right * m_quadTreeScale.x ));
	int yResult = y ^ ((int) ( nodeRect.Bottom * m_quadTreeScale.y ));

	int nodeLevel = LevelsCount - 1;

	while ( xResult + yResult > 0 )
	{
		xResult >>= 1;
		yResult >>= 1;
		nodeLevel--;
	}

	x >>= (LevelsCount - nodeLevel - 1);
	y >>= (LevelsCount - nodeLevel - 1);

	int nodeId = GetNodeId( x, y, nodeLevel );
	if ( nodeId < 0 || nodeId >= (int) m_quadTreeNodes.size() )
		nodeId = 0;

	return nodeId;
}


// =====================================================================================

