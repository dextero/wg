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
            buttonClose->SetText(L"Close", player->GetNumber(), 0);

            tooltip->SetHandler(this);
            tooltip->Show();
            mTooltip = tooltip;
            gGUI.RegisterInteractionHandler(this);
        }

        void Hide()
        {
            mTooltip->Hide();
            mTooltip->SetPriority(51); // dex: co to za 51, do czego to?
                                       // tox: jak naraz by sie zdarzylo, ze gracz(e) dotykaliby
                                       // dwoch roznych Signow/NPCow, to zeby interakcje sie nie
                                       // odpalaly w petli raz ta raz tamta. czy jakos tak. nie pamietam
                                       //    aha, juz pamietam - chodzi o to, zeby po kliknieciu w 'close'
                                       // nie otwieralo sie znowu okienki interakcji jesli gracz 'styka'
                                       // sie z npcem nadal
        }

        ~SignInteraction() {
            gGUI.UnregisterInteractionHandler(this);
        }

        void Update(float secondsPassed) {
            if (Maths::Length(mPlayer->GetPosition() - mObstacle->GetPosition()) > 1.5f) {
                mTooltip->Clear();
            }
        }

        virtual void OptionSelected(size_t selected) {
            if (selected == 0) {
                Hide();
            }
        };
};

#endif//__SIGN_INTERACTION_H__
