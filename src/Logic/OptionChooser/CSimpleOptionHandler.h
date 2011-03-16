#ifndef __CSIMPLE_OPTION_HANDLER_H__
#define __CSIMPLE_OPTION_HANDLER_H__

#include "IOptionChooserHandler.h"

class CSimpleOptionHandler : public IOptionChooserHandler
{
public:
	virtual void OptionSelected(size_t option) {
        fprintf(stderr, "OptionChosen: %d\n", option);
    };
};


#endif//__CSIMPLE_OPTION_HANDLER_H__
