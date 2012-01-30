#ifndef __INTERACTION_HANDLER_H__
#define __INTERACTION_HANDLER_H__

class InteractionHandler
{
    protected:
        CPlayer * playerControllingHandler;
    public:
        InteractionHandler(CPlayer * player = NULL) : playerControllingHandler(player) {}

        virtual void Update(float secondsPassed) {};
        virtual ~InteractionHandler() {};
        virtual void OptionSelected(size_t selected) {};
      
        CPlayer * GetControllingPlayer() { return playerControllingHandler; }
        void SetControllingPlayer(CPlayer * player) { playerControllingHandler = player; }
};


#endif//__INTERACTION_HANDLER_H__
