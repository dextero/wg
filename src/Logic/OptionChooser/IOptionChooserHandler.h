#ifndef __IOPTION_CHOOSER_HANDLER_H__
#define __IOPTION_CHOOSER_HANDLER_H__

#include "../../GUI/CInGameOptionChooser.h"

class IOptionChooserHandler
{
    private:
        CInGameOptionChooser * mChooser;
    public:        
        size_t mReferenceCounter;
        IOptionChooserHandler() : mChooser(NULL), mReferenceCounter(0) {};
        virtual ~IOptionChooserHandler() {}
        virtual void OptionSelected(size_t option) = 0;
        void SetChooser(CInGameOptionChooser * chooser) {
            mChooser = chooser;
        }
        void Hide() {
            if (mChooser) mChooser->Hide();
        }
};


#endif//__IOPTION_CHOOSER_HANDLER_H__
