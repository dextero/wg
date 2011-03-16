#ifndef __IOPTION_CHOOSER_HANDLER_H__
#define __IOPTION_CHOOSER_HANDLER_H__

class IOptionChooserHandler
{
public:
    virtual ~IOptionChooserHandler() {};
	virtual void OptionSelected(size_t option) = 0;
};


#endif//__IOPTION_CHOOSER_HANDLER_H__
