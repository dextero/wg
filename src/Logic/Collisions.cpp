#include "CPhysicalManager.h"
#include "CPhysical.h"
#include "CDetector.h"
#include "CPlayer.h"
#include "CLair.h"
#include "CBullet.h"
#include "../Map/CMapManager.h"
#include "../Utils/MathsFunc.h"
#include "Collisions.h"
#include "Effects/CEffectHandle.h"
#include "../Map/SceneManager/CQuadTreeNode.h"
#include "../Map/SceneManager/CSceneNode.h"
#include <iostream>
#include <sstream>
#include "../Logic/Items/CItem.h"
#include "MapObjects/CDoor.h"
#include "MapObjects/CHook.h"
#include "MapObjects/CRegion.h"
#include "MapObjects/CObstacle.h"
#include "../GUI/Messages/CMessageSystem.h"
#include "../Utils/StringUtils.h"
#include "../Rendering/CCamera.h"
#include "../GUI/Localization/CLocalizator.h"

using namespace Collisions;
using namespace Maths;
using namespace std;

bool Collisions::NeedToCheckCollision( CPhysical* physicalA, CPhysical* physicalB )
{
	if ( physicalA == NULL || physicalB == NULL )
		return false;
	physCategory catA = physicalA->GetCategory();
	physCategory catB = physicalB->GetCategory();

	if (( catA & PHYSICAL_MOVING ) &&
		//( !(catA == PHYSICAL_PLAYER && catB == PHYSICAL_BULLET) ) &&
		( !(catA == PHYSICAL_BULLET && catB == PHYSICAL_BULLET) ) &&
		( !(catA == PHYSICAL_BULLET && catB == PHYSICAL_ITEM) ) && 
		( !(catB == PHYSICAL_HOOK) )) 
		return true;
	return false;
}


void Collisions::LogicalResponse( CPhysical* physicalA, CPhysical* physicalB, bool& makePhysicalResponse )
{
	makePhysicalResponse = true;
	physCategory catA = physicalA->GetCategory();
	physCategory catB = physicalB->GetCategory();

    if ( (catA & PHYSICAL_ACTORS) && (catB & PHYSICAL_MOVING) )
        if (dynamic_cast<CActor*>(physicalA)->GetSpawnState() == CActor::ssDying){
            makePhysicalResponse = false;
            return;
        }
    if ( (catB & PHYSICAL_ACTORS) && (catA & PHYSICAL_MOVING) )
        if (dynamic_cast<CActor*>(physicalB)->GetSpawnState() == CActor::ssDying){
            makePhysicalResponse = false;
            return;
        }

	if ( catA == PHYSICAL_DETECTOR )
	{
		dynamic_cast<CDetector*>(physicalA)->HandleCollision(physicalB);
		makePhysicalResponse = false;
	}
	else if ( (catA & PHYSICAL_ACTORS) && catB == PHYSICAL_BULLET ) 
	{
        dynamic_cast<CBullet*>(physicalB)->HandleCollision(physicalA);
		makePhysicalResponse = false;
    }
	else if ( catA == PHYSICAL_MONSTER && catB == PHYSICAL_LAIR ) 
	{
		makePhysicalResponse = false;
    }
	else if ( catA == PHYSICAL_MONSTER && catB == PHYSICAL_ITEM ) 
	{
		makePhysicalResponse = false;
    }
	else if ( catA == PHYSICAL_BULLET && catB == PHYSICAL_LAIR ) 
	{
        dynamic_cast<CBullet*>(physicalA)->HandleCollision(physicalB);
		makePhysicalResponse = false;
    }
	else if ( catA == PHYSICAL_BULLET && catB == PHYSICAL_WALL ) 
	{
        dynamic_cast<CBullet*>(physicalA)->HandleCollision(physicalB);
		makePhysicalResponse = false;
	}
	else if ( catA == PHYSICAL_BULLET && catB == PHYSICAL_DOOR ) 
	{
        CDoor *door = dynamic_cast<CDoor*>(physicalB);
        if (!(door->LetThrough()))
            dynamic_cast<CBullet*>(physicalA)->HandleCollision(physicalB);
		makePhysicalResponse = false;
	}
    else if ( catA == PHYSICAL_PLAYER && catB == PHYSICAL_ITEM )
    {
        CItem *item = dynamic_cast<CItem*>(physicalB);
        item->Perform( dynamic_cast<CActor*>(physicalA) );
        item->MarkForDelete();

//        gMessageSystem.AddMessagef(gLocalizator.GetText("MSG_ITEM_PICKUP").c_str(), item->GetItemName().c_str() );
        makePhysicalResponse = false;
    } else if ( ((catA & PHYSICAL_MOVING) != 0) && catB == PHYSICAL_DOOR)
    {
        CDoor *door = dynamic_cast<CDoor*>(physicalB);
        makePhysicalResponse = !door->LetThrough();
    } else if ( catA == PHYSICAL_PLAYER && catB == PHYSICAL_REGION )
    {
        CDynamicRegion *region = dynamic_cast<CDynamicRegion*>(physicalB);
        makePhysicalResponse = false;
        region->Trigger(dynamic_cast<CPlayer*>(physicalA));
    } else if ( catB == PHYSICAL_REGION ) {
        makePhysicalResponse = false;
    } else if ( ( catA == PHYSICAL_BULLET ) && (catB == PHYSICAL_OBSTACLE )) {
		if (!(dynamic_cast<CObstacle*>(physicalB)->IsDestroyed())){
			dynamic_cast<CBullet*>(physicalA)->HandleCollision(physicalB);
		}
		makePhysicalResponse = false;
    }
}


void Collisions::PhysicalResponse( CPhysical* physicalA, CPhysical* physicalB, const sf::Vector2f& separateVector )
{
	if ( physicalB->GetCategory() & PHYSICAL_MOVING ) 
	{
		sf::Vector2f vectorAB = physicalB->GetPosition() - physicalA->GetPosition();
		float velA = Dot( vectorAB, physicalA->GetVelocity() ); if ( velA <= 0.0f ) velA = 0.01f;
		float velB = Dot(-vectorAB, physicalB->GetVelocity() ); if ( velB <= 0.0f ) velB = 0.01f;

		physicalA->SetPosition( physicalA->GetPosition() + separateVector * -velA / (velA + velB) );
		physicalB->SetPosition( physicalB->GetPosition() + separateVector * velB / (velA + velB) );
	}
	else 
	{
		physicalA->SetPosition( physicalA->GetPosition() + separateVector * -1.0f );
	}
}

void Collisions::CheckCollision( CPhysical* _physicalA, CPhysical* _physicalB )
{   
	CPhysical * physicalA = _physicalA;
	CPhysical * physicalB = _physicalB;

	if ( physicalA->GetCategory() > physicalB->GetCategory() )
		swap( physicalA, physicalB );

	if ( !NeedToCheckCollision( physicalA, physicalB ) )
		return;

	/* Oblicz wektor separacji */

	sf::Vector2f separateVector;

	if ( physicalA->GetCollisionShape() == SHAPE_CIRCLE && physicalB->GetCollisionShape() == SHAPE_CIRCLE )
	{
		separateVector = SeparateCircles(	physicalA->GetPosition(), physicalA->GetCircleRadius(),
											physicalB->GetPosition(), physicalB->GetCircleRadius() );
	}
	if ( physicalA->GetCollisionShape() == SHAPE_RECT && physicalB->GetCollisionShape() == SHAPE_CIRCLE )
	{
		separateVector = SeparateRectCircle(physicalA->GetPosition(),physicalA->GetRectSize(),
											physicalB->GetPosition(), physicalB->GetOldPosition(), physicalB->GetCircleRadius() );
	}
	if ( physicalA->GetCollisionShape() == SHAPE_CIRCLE && physicalB->GetCollisionShape() == SHAPE_RECT )
	{
		separateVector =-SeparateRectCircle(physicalB->GetPosition(),physicalB->GetRectSize(),
											physicalA->GetPosition(), physicalA->GetOldPosition(), physicalA->GetCircleRadius() );
	}
	if ( physicalA->GetCollisionShape() == SHAPE_RECT && physicalB->GetCollisionShape() == SHAPE_RECT )
	{
		separateVector = SeparateRects(	physicalA->GetPosition(), physicalA->GetRectSize(),
										physicalB->GetPosition(), physicalB->GetRectSize() );
	}

	/* Zareaguj na kolizje */

	if ( separateVector.x != 0.0f || separateVector.y != 0.0f )
	{
		bool makePhysicalResponse;
		LogicalResponse( physicalA, physicalB, makePhysicalResponse );
		if ( makePhysicalResponse )
			PhysicalResponse( physicalA, physicalB, separateVector );
	}
}



void Collisions::CheckAllCollisions()
{
	std::vector< CQuadTreeSceneNode > & quadTreeNodes = gScene.GetQuadTreeNodes();
	std::vector< CSceneNode* > * sceneNodesA, * sceneNodesB;

	// moving - moving
	
	for ( unsigned qtNodeIdA = 0; qtNodeIdA < quadTreeNodes.size(); qtNodeIdA++ )
	{
		sceneNodesA = quadTreeNodes[ qtNodeIdA ].GetMovingPhysicals();

		for ( unsigned sceneNodeIdA = 0; sceneNodeIdA < sceneNodesA->size(); sceneNodeIdA++ )
		{
			int	qtNodeIdB =		qtNodeIdA;
			int sceneNodeIdB =	sceneNodeIdA;
				sceneNodesB =	sceneNodesA;

			while ( true )
			{
				if ( --sceneNodeIdB < 0 ) {
					if ( ( qtNodeIdB = quadTreeNodes[ qtNodeIdB ].GetParentNodeId() ) < 0 ) break;
					else {
						sceneNodesB = quadTreeNodes[ qtNodeIdB ].GetMovingPhysicals();
						if ( ( sceneNodeIdB = (int) ( sceneNodesB->size() - 1 ) ) < 0 ) continue;
					}
				}
	
				CPhysical * physicalA = ( *sceneNodesA )[ sceneNodeIdA ]->GetPhysical();
				CPhysical * physicalB = ( *sceneNodesB )[ sceneNodeIdB ]->GetPhysical();
				CheckCollision( physicalA, physicalB );
			}
		}		
	}

	// moving - nomoving || moving - end of scene

	for ( unsigned qtNodeIdA = 0; qtNodeIdA < quadTreeNodes.size(); qtNodeIdA++ )
	{
		sceneNodesA = quadTreeNodes[ qtNodeIdA ].GetMovingPhysicals();

		for ( unsigned sceneNodeIdA = 0; sceneNodeIdA < sceneNodesA->size(); sceneNodeIdA++ )
		{
			int	qtNodeIdB =		qtNodeIdA;
				sceneNodesB =	quadTreeNodes[ qtNodeIdB ].GetNonMovingPhysicals();
			int sceneNodeIdB =	(int) sceneNodesB->size();
				
			while ( true )
			{
				if ( --sceneNodeIdB < 0 ) {
					if ( ( qtNodeIdB = quadTreeNodes[ qtNodeIdB ].GetParentNodeId() ) < 0 ) break;
					else {
						sceneNodesB = quadTreeNodes[ qtNodeIdB ].GetNonMovingPhysicals();
						if ( ( sceneNodeIdB = (int) ( sceneNodesB->size() - 1 ) ) < 0 ) continue;
					}
				}
	
				CPhysical * physicalA = ( *sceneNodesA )[ sceneNodeIdA ]->GetPhysical();
				CPhysical * physicalB = ( *sceneNodesB )[ sceneNodeIdB ]->GetPhysical();
				CheckCollision( physicalA, physicalB );
			}

			/* kolizja z brzegiem mapy i z kamera */

			CPhysical * physical = ( *sceneNodesA )[ sceneNodeIdA ]->GetPhysical();
			if ( physical != NULL )
			{
				sf::FloatRect sceneRect = quadTreeNodes[0].GetBoundingRect();
				sf::Vector2f position = physical->GetPosition();
				sf::Vector2f halfRect = physical->GetRectSize() * 0.5f;

				if ( physical->GetCategory() == PHYSICAL_BULLET )
				{
					if ( !sceneRect.Contains(position.x,position.y) )
						dynamic_cast<CBullet*>(physical)->HandleCollision(NULL);
				}
                else if ( ! (physical->GetCategory() == PHYSICAL_DETECTOR ))
				{
					position.x = Clamp( position.x, sceneRect.Left + halfRect.x, sceneRect.Right - halfRect.x );
					position.y = Clamp( position.y, sceneRect.Top + halfRect.y, sceneRect.Bottom - halfRect.y );
					physical->SetPosition( position );
				}

				if ( physical->GetCategory() == PHYSICAL_PLAYER && gCamera.GetCameraTask() == CCamera::TRAIL_PLAYERS )
				{
					sf::FloatRect cameraRect = gCamera.GetViewRectInGame();
					position.x = Clamp( position.x, cameraRect.Left + halfRect.x, cameraRect.Right - halfRect.x );
					position.y = Clamp( position.y, cameraRect.Top + halfRect.y, cameraRect.Bottom - halfRect.y );
					physical->SetPosition( position );
				}
			}

		}	

		if ( qtNodeIdA == 0 ) continue;
		sceneNodesA = quadTreeNodes[ qtNodeIdA ].GetNonMovingPhysicals();

		for ( unsigned sceneNodeIdA = 0; sceneNodeIdA < sceneNodesA->size(); sceneNodeIdA++ )
		{
			int	qtNodeIdB =		quadTreeNodes[ qtNodeIdA ].GetParentNodeId();
				sceneNodesB =	quadTreeNodes[ qtNodeIdB ].GetMovingPhysicals();
			int sceneNodeIdB =	(int) sceneNodesB->size();
				
			while ( true )
			{
				if ( --sceneNodeIdB < 0 ) {
					if ( ( qtNodeIdB = quadTreeNodes[ qtNodeIdB ].GetParentNodeId() ) < 0 ) break;
					else {
						sceneNodesB = quadTreeNodes[ qtNodeIdB ].GetMovingPhysicals();
						if ( ( sceneNodeIdB = (int) ( sceneNodesB->size() - 1 ) ) < 0 ) continue;
					}
				}
	
				CPhysical * physicalA = ( *sceneNodesA )[ sceneNodeIdA ]->GetPhysical();
				CPhysical * physicalB = ( *sceneNodesB )[ sceneNodeIdB ]->GetPhysical();
				CheckCollision( physicalA, physicalB );
			}
		}
	}
}
