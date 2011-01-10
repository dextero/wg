 #include "CPostProcessing.h"
#include "../../CGame.h"
#include <cstdio>

template<> pfx::CPostProcessing* CSingleton<pfx::CPostProcessing>::msSingleton = 0;

pfx::CPostProcessing::CPostProcessing() : type(pfx::PFX_TYPE::NONE)
{
	fprintf(stderr,"CPostProcessing::CPostProcessing()\n");
    gGame.AddFrameListener(this);
}
pfx::CPostProcessing::~CPostProcessing()
{
    it = pfx.end();
    while(it != pfx.begin())
        delete (it--)->second;
   
    pfx.erase(pfx.begin(), pfx.end());
}

bool pfx::CPostProcessing::SetType(pfx::PFX_TYPE::Type mode)
{
    type = mode;

    if ((mode == PFX_TYPE::SHADERS) && (!sf::PostFX::CanUsePostFX()))
    {
        mode = PFX_TYPE::NONE;
        return false;
    }

    it = pfx.begin();
    while(it != pfx.end())
        (it++)->second->Init(mode);

    return true;
}

void pfx::CPostProcessing::FrameStarted(float dt) 
{
    it = pfx.begin();

    switch(type)
    {
    case PFX_TYPE::NONE:
        return;
    default:
        while(it != pfx.end())
            (it++)->second->ApplyEffect(type, dt);
    };
}
