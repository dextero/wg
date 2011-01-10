#ifndef __ISCENE_NODE_H__
#define __ISCENE_NODE_H__

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

class CPhysical;
class CDisplayable;
class CQuadTreeSceneNode;

class CSceneNode
{
public:
	CSceneNode(void);
	~CSceneNode(void);

	inline void SetPhysical(CPhysical * _physical)	{ mPhysical = _physical; }
	inline CPhysical * GetPhysical()				{ return mPhysical; }

	inline void SetDisplayable(CDisplayable * _displayable)	{ mDisplayable = _displayable; }
	inline CDisplayable * GetDisplayable()					{ return mDisplayable; }

	inline void SetParentNodeId(int parent)			{ mParentNodeId = parent; }
	inline int GetParentNodeId()					{ return mParentNodeId; }

	inline void SetMapObjectDescriptorId(int id)	{ mMapObjectDescriptorId = id; }
	inline int GetMapObjectDescriptorId()			{ return mMapObjectDescriptorId; }

	inline void SetDoodahDescriptorId(int id)		{ mDoodahDescriptorId = id; }
	inline int GetDoodahDescriptorId()				{ return mDoodahDescriptorId; }

	void SetVisibility(bool visibility);
	bool GetVisibility();

	void SetColor(float r, float g, float b, float a);
	void SetPreviousColor();
	
	sf::FloatRect GetBoundingRect();
	sf::Vector2f GetPosition();
    
protected:
	CPhysical * mPhysical;
	CDisplayable * mDisplayable;
	float mPrevColorR, mPrevColorG, mPrevColorB, mPrevColorA;
	int mParentNodeId;
	int mMapObjectDescriptorId;
	int mDoodahDescriptorId;
};


#endif

