#include <cstdio>

#include "CInputHandler.h"
#include "CPlayerController.h"
#include "CMouseCaster.h"
#include "../Utils/Maths.h"
#include "../CGame.h"
#include "../Logic/CPlayerManager.h"
#include "../Logic/CPhysicalManager.h"
#include "../Logic/CPlayer.h"
#include "../Console/CConsole.h"
#include "../Utils/StringUtils.h"
#include "CBindManager.h"
#include "../Rendering/CCamera.h"
#include "../CGameOptions.h"
#include "../Commands/CCommands.h"
#include "../GUI/Localization/CLocalizator.h"
#include "../GUI/CInGameOptionChooser.h"
#include <iostream>
#include <math.h>

template<> CInputHandler* CSingleton<CInputHandler>::msSingleton = 0;

using namespace System::Input;
using namespace StringUtils;

CInputHandler::CInputHandler()
{
    fprintf( stderr, "CInputHandler::CInputHandler()\n" );
    
    gGame.AddFrameListener( this );
}

CInputHandler::~CInputHandler()
{
    fprintf( stderr, "CInputHandler::~CInputHandler()\n" );
}

void CInputHandler::FrameStarted( float secondsPassed ){

	for ( unsigned i = 0; i < gPlayerManager.GetPlayerCount(); i++ )
	{
		if ( CPlayer *p = gPlayerManager.GetPlayerByNumber(i) )
		if ( CPlayerController *pc = p->GetController() )
		{
			int keyMask, outMask;

			/* ruch */

			p->SetVelocity(sf::Vector2f(0.0f,0.0f));
			if (gConsole.GetVisible()) return;

			keyMask = CBindManager::KEY_HOLD | CBindManager::KEY_PRESS;

			bool a,b;
			a = ((gBindManagerByPlayer(i)->Check("MoveUp") & keyMask) != 0);
			b = ((gBindManagerByPlayer(i)->Check("MoveDown") & keyMask) != 0);
			pc->SetWalking(a,b);

			a = ((gBindManagerByPlayer(i)->Check("TurnRight") & keyMask) != 0);
			b = ((gBindManagerByPlayer(i)->Check("TurnLeft") & keyMask) != 0);
			pc->SetTurning(a,b);

			a = ((gBindManagerByPlayer(i)->Check("MoveRight") & keyMask) != 0);
			b = ((gBindManagerByPlayer(i)->Check("MoveLeft") & keyMask) != 0);
			pc->SetStrafing(a,b);

			bool north, east, south, west;
			north = ((gBindManagerByPlayer(i)->Check("MoveUpAbs") & keyMask) != 0);
			east = ((gBindManagerByPlayer(i)->Check("MoveRightAbs") & keyMask) != 0);
			south = ((gBindManagerByPlayer(i)->Check("MoveDownAbs") & keyMask) != 0);
			west = ((gBindManagerByPlayer(i)->Check("MoveLeftAbs") & keyMask) != 0);
			pc->SetWalkingAbsolute( north, east, south, west );

			/* obrót mysza */

			pc->SetMouseLook(gBindManagerByPlayer(i)->GetMouseLook());

			if (gBindManagerByPlayer(i)->GetMouseLook())
			{
				const sf::Input& in = gGame.GetRenderWindow()->GetInput();
				sf::Vector2f cursorPos( (float) in.GetMouseX() / gCamera.GetZoom(), (float) in.GetMouseY() / gCamera.GetZoom() );

				if (CMouseCaster* mc = gBindManagerByPlayer(i)->GetMouseCaster())
				if (gBindManagerByPlayer(i)->GetMouseCaster()->IsVisible())
				{
					cursorPos.x = (float) mc->GetSavedCursorPosition().x / gCamera.GetZoom();
					cursorPos.y = (float) mc->GetSavedCursorPosition().y / gCamera.GetZoom();
				}

				sf::Vector2f pMouse(gCamera.GetViewTopLeft().x + cursorPos.x, gCamera.GetViewTopLeft().y + cursorPos.y);
				sf::Vector2f pPlayer(p->GetPosition().x * 64.0f, p->GetPosition().y * 64.0f) ;	
				sf::Vector2f diff(pPlayer.x - pMouse.x, pMouse.y - pPlayer.y );

				float alpha = Maths::AngleBetween(Maths::VectorUp(), diff);
				pc->TurnBy(alpha - pc->GetTrueRot());
			}

			/* atak - kombinacje */
  			keyMask = CBindManager::KEY_PRESS;

            CInGameOptionChooser * oc = pc->GetOptionChooser();
            // todo: null check
            if (oc->IsVisible())
            {
                typedef std::map<std::string, size_t> MapType;
                MapType mapping;
                mapping["Slot-0"] = 0;
                mapping["Slot-1"] = 1;
                mapping["Slot-2"] = 2;
                mapping["AbiX-0"] = 0;
                mapping["AbiX-1"] = 1;
                mapping["AbiX-2"] = 2;
                mapping["AbiX-3"] = 3;
                mapping["Abi-0"] = 0;
                mapping["Abi-1"] = 1;
                mapping["Abi-2"] = 2;
                mapping["Abi-3"] = 3;

                for (MapType::const_iterator it = mapping.begin(); it != mapping.end(); it++) {
                    if (gBindManagerByPlayer(i)->Check(it->first) & keyMask) {
                        oc->OptionSelected(it->second);
                        gBindManagerByPlayer(i)->SetKeyState(it->first, false);
                    }
                }
            }
            else
            {
    			if (pc->AllowKeyHold()) keyMask |= CBindManager::KEY_HOLD;

    			if ((outMask = gBindManagerByPlayer(i)->Check("QuickAttack") & keyMask))
    				pc->AbiKeyPressed(KeyActionTypes::Both, 0, (outMask & CBindManager::KEY_HOLD) != 0); 

    			if ((outMask = gBindManagerByPlayer(i)->Check("Slot-0") & keyMask))
    				pc->AbiKeyPressed(KeyActionTypes::OnlySlot, 0, (outMask & CBindManager::KEY_HOLD) != 0); 
    			if ((outMask = gBindManagerByPlayer(i)->Check("Slot-1") & keyMask))
	    			pc->AbiKeyPressed(KeyActionTypes::OnlySlot, 1, (outMask & CBindManager::KEY_HOLD) != 0); 
		    	if ((outMask = gBindManagerByPlayer(i)->Check("Slot-2") & keyMask))
			    	pc->AbiKeyPressed(KeyActionTypes::OnlySlot, 2, (outMask & CBindManager::KEY_HOLD) != 0); 

    			if ((outMask = gBindManagerByPlayer(i)->Check("AbiX-0") & keyMask))
	    			pc->AbiKeyPressed(KeyActionTypes::OnlyAbi, 0, (outMask & CBindManager::KEY_HOLD) != 0);
		    	if ((outMask = gBindManagerByPlayer(i)->Check("AbiX-1") & keyMask))
			    	pc->AbiKeyPressed(KeyActionTypes::OnlyAbi, 1, (outMask & CBindManager::KEY_HOLD) != 0);
    			if ((outMask = gBindManagerByPlayer(i)->Check("AbiX-2") & keyMask))
	    			pc->AbiKeyPressed(KeyActionTypes::OnlyAbi, 2, (outMask & CBindManager::KEY_HOLD) != 0);
		    	if ((outMask = gBindManagerByPlayer(i)->Check("AbiX-3") & keyMask))
			    	pc->AbiKeyPressed(KeyActionTypes::OnlyAbi, 3, (outMask & CBindManager::KEY_HOLD) != 0);

    			if ((outMask = gBindManagerByPlayer(i)->Check("Abi-0") & keyMask))
	    			pc->AbiKeyPressed(KeyActionTypes::Both, 0, (outMask & CBindManager::KEY_HOLD) != 0);
		    	if ((outMask = gBindManagerByPlayer(i)->Check("Abi-1") & keyMask))
			    	pc->AbiKeyPressed(KeyActionTypes::Both, 1, (outMask & CBindManager::KEY_HOLD) != 0);
    			if ((outMask = gBindManagerByPlayer(i)->Check("Abi-2") & keyMask))
	    			pc->AbiKeyPressed(KeyActionTypes::Both, 2, (outMask & CBindManager::KEY_HOLD) != 0);
		    	if ((outMask = gBindManagerByPlayer(i)->Check("Abi-3") & keyMask))
			    	pc->AbiKeyPressed(KeyActionTypes::Both, 3, (outMask & CBindManager::KEY_HOLD) != 0);
            }

			/* mouse cast */

			const int press = CBindManager::KEY_PRESS;
			const int release = CBindManager::KEY_RELEASE;

			if (CMouseCaster* mc = gBindManagerByPlayer(i)->GetMouseCaster())
			{
				int currentKey = gBindManagerByPlayer(i)->GetMouseCaster()->GetCurrentKey();
				int lastKey = gBindManagerByPlayer(i)->GetMouseCaster()->GetLastKey();
				if ( currentKey >= 0 && pc->AllowKeyHold() == (lastKey == currentKey) )
					pc->AbiKeyPressed(KeyActionTypes::Both, currentKey, pc->AllowKeyHold());

				if (gBindManagerByPlayer(i)->Check("SpecialAttack") & press)		mc->Cast();
				else if (gBindManagerByPlayer(i)->Check("SpecialAttack") & release)	mc->Stop();
				else if (pc->AbilityActivatedJustNow())								mc->Focus();

			}
		}
	}
}

void CInputHandler::SwitchControls(){
    CPlayer *p = gPlayerManager.GetPlayerByNumber(0);
    if (p==NULL) return;
    CPlayerController *pc = p->GetController();
    if (pc==NULL) return;
    pc->SwitchControls();
}

void CInputHandler::InitControlSets()
{
	// O_o
}
