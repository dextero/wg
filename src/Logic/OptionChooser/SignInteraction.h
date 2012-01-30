#ifndef __SIGN_INTERACTION_H__
#define __SIGN_INTERACTION_H__

#include "InteractionHandler.h"
#include "../../GUI/CRoot.h"
#include "../../GUI/CTextArea.h"
#include "../../GUI/CWindow.h"
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
            description->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
            description->SetText(StringUtils::ConvertToWString(title));
            description->SetPosition(5.f, 5.f, 90.f, 90.f);
            gGUI.UnregisterObject(description);
            description->SetVisible(true);
            tooltip->SetHandler(this);
            tooltip->Show();
            mTooltip = tooltip;
            gGUI.RegisterInteractionHandler(this);
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
