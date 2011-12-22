#ifndef __CPHYSICAL_MANAGER_H__
#define __CPHYSICAL_MANAGER_H__

#include "../Utils/CSingleton.h"
#include "../IFrameListener.h"

#include <string>
#include <map>

#define gPhysicalManager CPhysicalManager::GetSingleton()

// klasy, ktore moze produkowac manager... sporo 
class CDetector;
class CPhysical;
class CActor;
class CEnemy;
class CNpc;
class CPlayer;
class CBullet;
class CLair;
class CWall;
class CLoot;
class CDoor;
class CDynamicRegion;
class CHook;
class CObstacle;
struct SItemTemplate;

class CPhysicalManager : public CSingleton< CPhysicalManager >, public IFrameListener
{
public:
    CPhysicalManager();
    virtual ~CPhysicalManager();

    // Liosan 23-02: czyszczenie 
    virtual void FrameStarted( float secondsPassed );
	virtual bool FramesDuringPause(EPauseVariant pv) { return pv == pvLogicOnly; };

    CPhysical* CreatePhysical( const std::wstring& uniqueId = L"" );
	CDetector* CreateDetector( const std::wstring& uniqueId = L"" );
    CActor *CreateActor( const std::wstring &uniqueId = L"");
    CEnemy *CreateEnemy( const std::wstring &uniqueId = L"");
	CNpc *CreateNpc( const std::wstring &uniqueId = L"");
    CPlayer *CreatePlayer( const std::wstring &uniqueId = L"");
    CBullet *CreateBullet( const std::wstring &uniqueId = L"");
	CLair *CreateLair( const std::wstring &uniqueId = L"");
	CWall *CreateWall( const std::wstring &uniqueId = L"");
	CHook *CreateHook( const std::wstring &uniqueId = L"");
    CLoot *CreateLoot( const std::wstring &uniqueId = L"");
    CDoor *CreateDoor( const std::wstring &uniqueId = L"");
    CDynamicRegion *CreateRegion( const std::wstring &uniqueId = L"");
    CObstacle *CreateObstacle( const std::wstring &uniqueId = L"");

    void DestroyPhysical( CPhysical* physical );
    void DestroyPhysical( const std::wstring& id );
    void Clear( bool forceDestroyInstantly = false ); // tox, 11 kwietnia: usuwa wszystkie physicale z gry

    const std::map< std::wstring, CPhysical* >& GetNamedPhysicals();
    const std::vector< CPhysical *>& GetPhysicals();

    CPhysical* GetPhysicalById( const std::wstring& id );

private:
    template<class T> T* Create(const std::wstring &uniqueId);
    
    std::map< std::wstring, CPhysical* > mNamedPhysicals;
    std::vector<CPhysical*> mPhysicals;
    void CheckUniqueId( std::wstring& newUniqueId ); 

#ifdef __EDITOR__
    friend class CEditorWnd;
#endif
};

#endif//__CPHYSICAL_MANAGER_H__
