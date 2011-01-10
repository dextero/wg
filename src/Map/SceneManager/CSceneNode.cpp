#include "CSceneNode.h"
#include "CQuadTreeNode.h"
#include "../../Logic/CPhysical.h"
#include "../../Logic/CPhysicalManager.h"
#include "../../Rendering/CDisplayable.h"
#include "../../Rendering/CDrawableManager.h"
#include "../../Utils/MathsFunc.h"
#include "../../Utils/StringUtils.h"
#include "../CMap.h"
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <iostream>

CSceneNode::CSceneNode(void) :	mPhysical(NULL), mDisplayable(NULL), mPrevColorR(1.0f), mPrevColorG(1.0f),
								mPrevColorB(1.0f), mPrevColorA(1.0f), mParentNodeId(-1), 
								mMapObjectDescriptorId(-1), mDoodahDescriptorId(-1)
{
}

CSceneNode::~CSceneNode(void)
{
}

void CSceneNode::SetVisibility(bool visibility)
{
	if ( mPhysical != NULL && mPhysical->GetDisplayable() != NULL )
		mPhysical->GetDisplayable()->SetCanDraw(visibility);

	if ( mDisplayable )
		mDisplayable->SetCanDraw(visibility);
}

bool CSceneNode::GetVisibility()
{
	if ( mPhysical != NULL && mPhysical->GetDisplayable() != NULL )
		return mPhysical->GetDisplayable()->GetCanDraw();

	if ( mDisplayable )
		return mDisplayable->GetCanDraw();

	return false;
}

sf::FloatRect CSceneNode::GetBoundingRect()
{
	sf::FloatRect out;
	sf::Vector2f position(0.0f,0.0f);
	sf::Vector2f halfSize(0.0f,0.0f);

	if ( mPhysical != NULL )
	{
		position = mPhysical->GetPosition();
		if ( mPhysical->GetDisplayable() != NULL )
		{
			halfSize.x = mPhysical->GetDisplayable()->GetScale().x * 0.5f;
			halfSize.y = mPhysical->GetDisplayable()->GetScale().y * 0.5f;
		}
		else										
			halfSize = mPhysical->GetRectSize() * 0.5f;
	}
	else if ( mDisplayable != NULL )
	{
		position = mDisplayable->GetPosition() / (float) Map::TILE_SIZE;
		halfSize.x = halfSize.y = std::max( mDisplayable->GetSFSprite()->GetSize().x, mDisplayable->GetSFSprite()->GetSize().y );
		halfSize.x *= mDisplayable->GetScale().x * 0.5f / (float) Map::TILE_SIZE;
		halfSize.y *= mDisplayable->GetScale().y * 0.5f / (float) Map::TILE_SIZE;
	}

	out.Left =	 position.x - halfSize.x;
	out.Top =	 position.y - halfSize.y;
	out.Right =  position.x + halfSize.x;
	out.Bottom = position.y + halfSize.y;

	return out;
}

sf::Vector2f CSceneNode::GetPosition()
{
	if ( mPhysical )			return mPhysical->GetPosition();
	else if ( mDisplayable )	return mDisplayable->GetPosition() / (float) Map::TILE_SIZE;
	else						return sf::Vector2f( 0.0f, 0.0f );
}

void CSceneNode::SetColor(float r, float g, float b, float a)
{
	if ( mPhysical )
	{
		mPhysical->GetColor( &mPrevColorR, &mPrevColorG, &mPrevColorB, &mPrevColorA ); 
		mPhysical->SetColor( r, g, b, a );
	}
	else if ( mDisplayable )
	{
		mDisplayable->GetColor( &mPrevColorR, &mPrevColorG, &mPrevColorB, &mPrevColorA ); 
		mDisplayable->SetColor( r, g, b, a );
	}
}

void CSceneNode::SetPreviousColor()
{
	SetColor( mPrevColorR, mPrevColorG, mPrevColorB, mPrevColorA );
}

