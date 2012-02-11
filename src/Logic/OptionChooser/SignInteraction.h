#ifndef __SIGN_INTERACTION_H__
#define __SIGN_INTERACTION_H__

#include "InteractionHandler.h"
#include "../../GUI/CRoot.h"
#include "../../GUI/CTextArea.h"
#include "../../GUI/CWindow.h"
#include "../../GUI/CButton.h"
#include "../CPlayer.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/Maths.h"

class SignInteraction : public InteractionHandler
{
    protected:
        CPhysical * mPlayer;
        CPhysical * mObstacle;
        CInteractionTooltip * mTooltip;

    public:        
        SignInteraction(CInteractionTooltip * tooltip, const std::string & title, CPlayer * player, CPhysical * obstacle) :
            InteractionHandler(player),
                mPlayer(player),
                mObstacle(obstacle),
                mTooltip(tooltip)
        {
            tooltip->Clear();
            GUI::CTextArea * description = tooltip->GetCanvas()->CreateTextArea("description");
            description->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"), 1.5f);
            description->SetText(StringUtils::ConvertToWString(title));
            description->SetPosition(5.f, 5.f, 90.f, 90.f);
            description->SetVisible(true);
            gGUI.UnregisterObject(description);
            
            GUI::CButton * buttonClose = tooltip->GetCanvas()->CreateButton("close");
            buttonClose->SetImage("data/GUI/btn-up.png", "data/GUI/btn-hover.png", "data/GUI/btn-down.png");
            buttonClose->SetFont(gGUI.GetFontSetting("FONT_MENU_BUTTON"));
            buttonClose->SetPosition(35.0f, 87.0f, 30.0f, 6.0f);
            buttonClose->SetCenter(true);
            buttonClose->GetClickCallback()->bind(this, &SignInteraction::Hide);
            buttonClose->SetText(L"Close");

            tooltip->SetHandler(this);
            tooltip->Show();
            mTooltip = tooltip;
            gGUI.RegisterInteractionHandler(this);
        }

        void Hide()
        {
            mTooltip->Hide();
            mTooltip->SetPriority(51); // dex: co to za 51, do czego to?
        }

        ~SignInteraction() {
            gGUI.UnregisterInteractionHandler(this);
        }

        void Update(float secondsPassed) {
            if (Maths::Length(mPlayer->GetPosition() - mObstacle->GetPosition()) > 1.5f) {
                mTooltip->Hide();
            }
        }
};

#endif//__SIGN_INTERACTION_H__
