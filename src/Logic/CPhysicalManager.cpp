#include "CPhysicalManager.h"
#include "../Console/CConsole.h"
#include "CPhysical.h"
#include "CDetector.h"
#include "CPlayer.h"
#include "CEnemy.h"
#include "CNpc.h"
#include "CLair.h"
#include "CBullet.h"
#include "MapObjects/CWall.h"
#include "MapObjects/CHook.h"
#include "MapObjects/CDoor.h"
#include "MapObjects/CRegion.h"
#include "MapObjects/CObstacle.h"
#include "Loots/CLoot.h"
#include "../Utils/StringUtils.h"
#include "../CGame.h"
#include <sstream>

template<> CPhysicalManager* CSingleton<CPhysicalManager>::msSingleton = 0;


CPhysicalManager::CPhysicalManager()
{
    fprintf( stderr, "CPhysicalManager::CPhysicalManager()\n" );

    gGame.AddFrameListener( this );
}

CPhysicalManager::~CPhysicalManager()
{
    fprintf( stderr, "CPhysicalManager::~CPhysicalManager()\n" );

    Clear(true);
}

void CPhysicalManager::FrameStarted( float secondsPassed )
{
    // Liosan, 23-02: odroczone niszczenie obiektow
    std::vector<CPhysical*> dirty;
    std::vector< CPhysical* >::iterator it;

    for ( it = mPhysicals.begin() ; it != mPhysicals.end() ; it++ )
    {
        if ((*it)->mReadyForDestruction)
            dirty.push_back(*it);
    }

    for (unsigned int i = 0; i < dirty.size(); i++)
        DestroyPhysical(dirty[i]);
}

template<class T>
T* CPhysicalManager::Create(const std::wstring &uniqueId) {
    std::wstring newUniqueId = uniqueId;
    if (uniqueId != L""){
        CheckUniqueId(newUniqueId);

        if (mNamedPhysicals.find(newUniqueId) != mNamedPhysicals.end() ) {
            gConsole.Printf( L"PhysicalsManager::Create<T> - %ls key already in mNamedPhysicals map", newUniqueId.c_str() );
            return NULL;
        }

	    mNamedPhysicals[ newUniqueId ] = NULL; // zarezerwowanie nazwy przed utworzeniem obiektu
    }
    T* obj = new T( newUniqueId );
    obj->mPhysicalManagerIndex = (int)mPhysicals.size();
    mPhysicals.push_back(obj);
    if (uniqueId != L"")
        mNamedPhysicals[ newUniqueId ] = obj;
    return obj;
}

CPhysical *CPhysicalManager::CreatePhysical( const std::wstring& uniqueId){
    return Create<CPhysical>(uniqueId);
}

CDetector *CPhysicalManager::CreateDetector( const std::wstring& uniqueId){
	return Create<CDetector>(uniqueId);
}

CActor *CPhysicalManager::CreateActor( const std::wstring &uniqueId){
    return Create<CActor>(uniqueId);
}

CEnemy *CPhysicalManager::CreateEnemy( const std::wstring &uniqueId){
    return Create<CEnemy>(uniqueId);
}

CNpc *CPhysicalManager::CreateNpc( const std::wstring &uniqueId){
	return Create<CNpc>(uniqueId);
}

CPlayer *CPhysicalManager::CreatePlayer( const std::wstring &uniqueId){
    return Create<CPlayer>(uniqueId);
}

CBullet *CPhysicalManager::CreateBullet( const std::wstring &uniqueId){
    return Create<CBullet>(uniqueId);
}

CLair *CPhysicalManager::CreateLair( const std::wstring &uniqueId){
    return Create<CLair>(uniqueId);
}

CWall *CPhysicalManager::CreateWall( const std::wstring &uniqueId){
    return Create<CWall>(uniqueId);
}

CHook *CPhysicalManager::CreateHook( const std::wstring &uniqueId){
    return Create<CHook>(uniqueId);
}

CLoot *CPhysicalManager::CreateLoot( const std::wstring &uniqueId) {
    return Create<CLoot>(uniqueId);
}

CDoor *CPhysicalManager::CreateDoor(const std::wstring &uniqueId){
    return Create<CDoor>(uniqueId);
}

CDynamicRegion *CPhysicalManager::CreateRegion(const std::wstring &uniqueId){
    return Create<CDynamicRegion>(uniqueId);
}

CObstacle *CPhysicalManager::CreateObstacle( const std::wstring &uniqueId){
    return Create<CObstacle>(uniqueId);
}

void CPhysicalManager::DestroyPhysical( CPhysical* physical )
{
    if (physical->mPhysicalManagerIndex >= 0){
        mPhysicals[physical->mPhysicalManagerIndex]=mPhysicals[mPhysicals.size()-1];
        mPhysicals[physical->mPhysicalManagerIndex]->mPhysicalManagerIndex = physical->mPhysicalManagerIndex;
        mPhysicals.pop_back();
        physical->mPhysicalManagerIndex = -1;
        DestroyPhysical(physical->GetUniqueId());
        delete physical;
    }
}

void CPhysicalManager::DestroyPhysical( const std::wstring& id )
{
    if ( mNamedPhysicals.find( id ) != mNamedPhysicals.end() )
    {
        CPhysical* physical = mNamedPhysicals[ id ];
        mNamedPhysicals.erase( mNamedPhysicals.find( id ) );
        DestroyPhysical(physical);
    }
}

void CPhysicalManager::Clear( bool forceDestroyInstantly )
{
    std::vector< CPhysical* >::iterator it;

    for ( it = mPhysicals.begin() ; it != mPhysicals.end() ; it++ )
    {
        (*it)->MarkForDelete();
    }

    if ( forceDestroyInstantly )
    {
        FrameStarted( 0.0f );
    }
}

const std::map< std::wstring, CPhysical* >& CPhysicalManager::GetNamedPhysicals()
{
    return mNamedPhysicals;
}

const std::vector<CPhysical *> &CPhysicalManager::GetPhysicals()
{
    return mPhysicals;
}

CPhysical* CPhysicalManager::GetPhysicalById( const std::wstring& id )
{
    if ( mNamedPhysicals.find( id ) != mNamedPhysicals.end() )
        return mNamedPhysicals[ id ];
    else
    {
        //gConsole.Printf( L"PhysicalsManager::GetPhysicalById - %ls key not found in mNamedPhysicals map", id.c_str() );
        return NULL;
    }
}

void CPhysicalManager::CheckUniqueId( std::wstring& newUniqueId ){
	static long physicalCount = 0;
    if ( (newUniqueId == L"" ) || (mNamedPhysicals.count(newUniqueId) > 0))
        do{
            newUniqueId = L"physical" + StringUtils::ToWString(physicalCount++);
        } while ( mNamedPhysicals.find( newUniqueId ) != mNamedPhysicals.end() );
}
