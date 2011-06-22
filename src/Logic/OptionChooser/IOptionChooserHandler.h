#ifndef __IOPTION_CHOOSER_HANDLER_H__
#define __IOPTION_CHOOSER_HANDLER_H__

#include "../../GUI/CInGameOptionChooser.h"
#include "../CPlayer.h"

class IOptionChooserHandler
{
    protected:
        CInGameOptionChooser * mChooser;
        CPlayer * mPlayer;
    public:        
        size_t mReferenceCounter;
        IOptionChooserHandler() : mChooser(NULL), mPlayer(NULL), mReferenceCounter(0) {};
        virtual ~IOptionChooserHandler() {}
        virtual void OptionSelected(size_t option) = 0;
        virtual void Update(float secondsPassed) {}

        void SetChooser(CInGameOptionChooser * chooser) {
            if (mChooser) {
                mChooser->Hide();
            }
            mChooser = chooser;
            if (chooser) {
                mPlayer = chooser->GetPlayer();
            } else {
                mPlayer = NULL;
            }
        }
        void Hide() {
            if (mChooser) mChooser->Hide();
        }
};


#endif//__IOPTION_CHOOSER_HANDLER_H__
