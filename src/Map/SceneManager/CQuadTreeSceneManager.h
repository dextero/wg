#ifndef __CQUADTREESCENEMANAGER_H__
#define __CQUADTREESCENEMANAGER_H__

#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "../../Console/CConsole.h"
#include "CQuadTreeNode.h"
#include "../../Logic/PhysicalCategories.h"

class CQuadTreeSceneManager
{
public:

	CQuadTreeSceneManager();
	~CQuadTreeSceneManager();

	void Initialize(sf::Vector2<int> size);
	void AddSceneNode(CSceneNode * node);
	void RemoveSceneNode(CSceneNode * node,bool eraseContent=false);
	void ClearScene();
	void UpdateScene();	
	void CullVisibility( const sf::FloatRect & viewRectangle);
    void SceneStats( int & currentDrawObjectsCount, int & totalObjectsCount );
	void GetPhysicalsInRadius( const sf::Vector2f& pos, float radius, int category, ESide side, std::vector<CPhysical*>& outPhysicals );
	void GetPhysicalsBetweenPoints( const sf::Vector2f& a, const sf::Vector2f& b, float tolerance, int category, std::vector<CPhysical*>& outPhysicals );
	bool ContainsPhysicals( int category );
	CSceneNode* GetDisplayableAt( const sf::Vector2f& v );
	CSceneNode* GetPhysicalAt( const sf::Vector2f& v );
	std::vector<CQuadTreeSceneNode> & GetQuadTreeNodes();

    int GetPhysicalCount(int type);

	sf::Vector2<int> GetSize();

private:
	void BuildLogicalStructure();
	void DropResources();
	int GetNodeIdContaining( CSceneNode * node );

	inline int GetNodeId( int x, int y, int level ) {
		return ( (1<<2*level) - 1 ) / 3 + y * (1<<level) + x;	// (4^level - 1)/3 + y*2^level + x
	}

	inline int GetPhysicalCategoryId( int category ) {
		int cat = category, id = 0;
		while ( cat > 0 ) { cat >>= 1; ++id; }
		return id;
	}

	static const int LevelsCount = 4;
	sf::Vector2<int> m_size;
	sf::Vector2<float> m_quadTreeScale;

	std::vector<CQuadTreeSceneNode> m_quadTreeNodes;

	int m_currentDrawObjectsCount;
	int m_totalObjectsCount;
	int m_totalAddedObjectsCount;
	int m_physicalCount[ 32 ];
};

#endif
