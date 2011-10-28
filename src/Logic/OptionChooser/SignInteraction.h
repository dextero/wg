#ifndef __SIGN_INTERACTION_H__
#define __SIGN_INTERACTION_H__

//#include "../../GUI/CInGameOptionChooser.h"
//#include "../CPlayer.h"

#include "InteractionHandler.h"
#include "../../GUI/CRoot.h"
#include "../../GUI/CTextArea.h"
#include "../../GUI/CWindow.h"
#include "../../Utils/StringUtils.h"

class SignInteraction : public InteractionHandler
{
    protected:
//      CInGameOptionChooser * mChooser;
//      CPlayer * mPlayer;

    public:        
        SignInteraction(CInteractionTooltip * tooltip, const std::string & title) {
            GUI::CTextArea * description = tooltip->GetCanvas()->CreateTextArea("description");
            description->SetFont(gGUI.GetFontSetting("FONT_DEFAULT"));
            description->SetText(StringUtils::ConvertToWString(title));
            description->SetPosition(5.f, 5.f, 90.f, 90.f);
            gGUI.UnregisterObject(description);
            description->SetVisible(true);
            tooltip->SetHandler(this);
            tooltip->Show();
        }

//      size_t mReferenceCounter;
//      IOptionChooserHandler() : mChooser(NULL), mPlayer(NULL), mReferenceCounter(0) {};
//      virtual ~IOptionChooserHandler() {}
//      virtual void OptionSelected(size_t option) = 0;
//      virtual void Update(float secondsPassed) {}

//      void SetChooser(CInGameOptionChooser * chooser) {
//          if (mChooser) {
//              mChooser->Hide();
//          }
//          mChooser = chooser;
//          if (chooser) {
//              mPlayer = chooser->GetPlayer();
//          } else {
//              mPlayer = NULL;
//          }
//      }
//      void Hide() {
//          if (mChooser) mChooser->Hide();
//      }
      
};


#endif//__SIGN_INTERACTION_H__
