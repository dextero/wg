#ifndef CAISCHEMEMANAGER_H_
#define CAISCHEMEMANAGER_H_

#include <string>
#include <map>

#include "../../Utils/CSingleton.h"
#include "../../IFrameListener.h"

#define gAISchemeManager CAISchemeManager::GetSingleton()

class CAIScheme;

class CAISchemeManager : public CSingleton<CAISchemeManager>, IFrameListener
{
public:
	CAISchemeManager();
	virtual ~CAISchemeManager();
	
    virtual void FrameStarted( float secondsPassed );
	virtual bool FramesDuringPause(EPauseVariant pv) { return pv==pvLogicOnly; };

	void RegisterScheme(const std::wstring &name, CAIScheme *scheme);
	void UnregisterScheme(const std::wstring &name);
	
	CAIScheme *GetScheme(const std::wstring &name);

	void SetBoidDebug(bool on);
private:
    std::map<std::wstring,CAIScheme *> schemes;
};

#endif /*CAISCHEMEMANAGER_H_*/

