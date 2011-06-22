#ifndef __CPOSTPROCESSING_H__
#define __CPOSTPROCESSING_H__

#include "../../Utils/CSingleton.h"
#include "../../IFrameListener.h"
#include "CPostFX.h"

#include <map>

#define gPostProcessing pfx::CPostProcessing::GetSingleton()

namespace pfx
{
    class CPostProcessing : public CSingleton<CPostProcessing>, public IFrameListener
    {
    public:
        CPostProcessing();
        ~CPostProcessing();

        virtual void FrameStarted(float);
        virtual bool FramesDuringPause(EPauseVariant pv) { return pv == pvLogicOnly; }; 
        bool SetType(PFX_TYPE::Type mode);
        void AddEffect(const std::string &name, CPostFX * effect) { pfx[name] = effect; }
        CPostFX *GetEffect(const std::string &name) { return (pfx.count(name) != 0? pfx[name] : NULL); }
    private:
        PFX_TYPE::Type type;
        std::map<std::string, CPostFX*> pfx;
        std::map<std::string, CPostFX*>::iterator it;
    };
};

#endif //__CPOSTPROCESSING_H__//
