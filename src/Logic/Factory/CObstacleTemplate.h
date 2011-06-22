/* Wzorzec sciany */

#ifndef _COBSTACLETEMPLATE_H_
#define _COBSTACLETEMPLATE_H_

#include "CPhysicalTemplate.h"
#include "../MapObjects/CObstacle.h"
#include <string>
#include <vector>

class CStats;
class CXml;
struct SAnimation;
class CObstacle;

class CObstacleTemplate : public CPhysicalTemplate
{
public:
    CObstacleTemplate(): mDefaultAnim(NULL), mDeathAnim(NULL), mMinStats(NULL), mMaxStats(NULL) {}
	~CObstacleTemplate() { }

	/* IResource */

	std::string GetType();
	bool Load(CXml &xml);
	void Drop();

	/* ========= */

	virtual void PrepareEditorPreviewer( CDisplayable* d );

    CObstacle* Create(std::wstring id = L"");

    virtual bool CoreMapObject() const { return false; }
private:
    SAnimation *mDefaultAnim,*mDeathAnim;
	CStats *mMinStats,*mMaxStats;
    std::string mImage;
};

#endif

