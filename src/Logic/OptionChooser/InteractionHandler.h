#ifndef __INTERACTION_HANDLER_H__
#define __INTERACTION_HANDLER_H__

class InteractionHandler
{
    protected:
    public:        
        virtual void Update(float secondsPassed) {};
        virtual ~InteractionHandler() {};
        virtual void OptionSelected(size_t selected) {};
      
};


#endif//__INTERACTION_HANDLER_H__
