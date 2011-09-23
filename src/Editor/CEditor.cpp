#include "CEditor.h"
#include "CEditorResourceSet.h"
#include "IEditorResource.h"
#include "CTileResource.h"
#include "CDoodahResource.h"
#include "CPhysicalResource.h"
#include "CRegionResource.h"
#include "../CGame.h"
#include "../Rendering/CCamera.h"
#include "../Map/CMapManager.h"
#include "../Map/CMap.h"
#include "../Map/SceneManager/CSceneNode.h"
#include "../Map/SceneManager/CQuadTreeSceneManager.h"
#include "../ResourceManager/CResourceManager.h"
#include "../Rendering/Animations/SAnimationState.h"
#include "../Rendering/CCamera.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/CDisplayable.h"
#include "../Utils/Maths.h"
#include "../GUI/Editor/CEditorScreens.h"
#include "../Logic/Factory/CPhysicalTemplate.h"
#include "../Logic/MapObjects/CRegion.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <math.h>

template<> CEditor* CSingleton<CEditor>::msSingleton = 0;

CEditor::CEditor():
mRunning(false),
mMode(PLACEMENT_MODE),
mMouseLeftDown(false),
mMouseRightDown(false),
mSavedMousePos(0.0f,0.0f),
mSavedCameraPos(0.0f,0.0f),
mTiles(NULL),
mDoodahs(NULL),
mPhysicals(NULL),
mRegion(NULL),
mSelected(NULL),
mSelectedToErase(NULL),
mRotation(0.0f),
mRotationSpeed(0.0f),
mScale(1.0f),
mScaleSpeed(0.0f),
mZIndexFlag(false),
mShowInvisibleWalls(false),
mIsModifying(false),
mUseLockedMousePos(false),
mLockedMousePos(0.f, 0.f),
mWorkspace(0.0f,0.0f,1.0f,1.0f),
mEditorScreens(NULL)
{
    gGame.AddFrameListener( this );
	gGame.AddKeyListener( this );
    gGame.AddMouseListener( this );

    mPreviewer = gDrawableManager.CreateDisplayable();

    mRegion = new CRegionResource();
}

CEditor::~CEditor(){
#ifndef __EDITOR__
    for (std::vector<IEditorResource*>::const_iterator it = mTiles->GetResources().begin(); it != mTiles->GetResources().end(); ++it)
        delete dynamic_cast<CTileResource*>(*it);
    for (std::vector<IEditorResource*>::const_iterator it = mDoodahs->GetResources().begin(); it != mDoodahs->GetResources().end(); ++it)
        delete dynamic_cast<CDoodahResource*>(*it);
    for (std::vector<IEditorResource*>::const_iterator it = mPhysicals->GetResources().begin(); it != mPhysicals->GetResources().end(); ++it)
        delete dynamic_cast<CPhysicalResource*>(*it);
#else
    for (std::map<const std::string, IEditorResource*>::const_iterator it = mTiles->GetResourcesMap().begin(); it != mTiles->GetResourcesMap().end(); ++it)
        delete dynamic_cast<CTileResource*>(it->second);
    for (std::map<const std::string, IEditorResource*>::const_iterator it = mDoodahs->GetResourcesMap().begin(); it != mDoodahs->GetResourcesMap().end(); ++it)
        delete dynamic_cast<CDoodahResource*>(it->second);
    for (std::map<const std::string, IEditorResource*>::const_iterator it = mPhysicals->GetResourcesMap().begin(); it != mPhysicals->GetResourcesMap().end(); ++it)
        delete dynamic_cast<CPhysicalResource*>(it->second);
#endif

    delete mTiles;
    delete mDoodahs;
    delete mPhysicals;
    delete mRegion;
}

void CEditor::MousePressed( const sf::Event::MouseButtonEvent &e ){
	if (!mRunning) return;

	if (MouseInWorkspace())
	{
		if (e.Button == sf::Mouse::Left)
		{
			if (mMode == PLACEMENT_MODE && mSelected)
			{
				mMouseLeftDown = true;

                // do regionow potrzeba zapisac info o deskryptorze w CEditorWnd, a zeby sie do niego dobrac,
                // to Place() musi byc w tamtej klasie.. (patrz: CEditorWnd.cpp, OnSfmlWindowLeftDown)
                if (dynamic_cast<CRegionResource*>(mSelected) == NULL)
				    mSelected->Place(GetLockedMousePos(),mRotation,mScale,mZIndexFlag);

                // jesli przestawialismy, zmieniamy tryb na modyfikacje
                if (mIsModifying)
                    SetMode(MODIFYING_MODE);
			}
            // MODIFYING_MODE tez korzysta z mSelectedToErase, zeby podswietlic obiekt, ktory zostanie wybrany
			else if ((mMode == DOODAHS_ERASING_MODE || mMode == PHYSICALS_ERASING_MODE || mMode == MODIFYING_MODE) && mSelectedToErase)
			{
				if (Map::CMap* map = gMapManager.GetCurrent())
				{

// w ifdefie, bo SetSelected(string) nie istnieje w "normalnej" grze
#ifdef __EDITOR__
                    // jesli mamy tryb modyfikowania, to przed usunieciem obiektu ustawiamy go jako pedzel
                    if (mMode == MODIFYING_MODE)
                    {
                        // po przestawieniu wracamy do MODIFYING_MODE
                        mIsModifying = true;

                        if (CDisplayable* displayable = mSelectedToErase->GetDisplayable())
                        {
                            // doodah - animowany?
                            if (displayable->GetAnimationState())
                            {
                                SetSelected(displayable->GetAnimationState()->GetCurrentFrame().imageName);
                                mScale = displayable->GetScale().x;
                            }
                            else // albo i nie...?
                            {
                                if (displayable->GetSFSprite())
                                {
                                    if (displayable->GetImageName() != "")
                                        SetSelected(displayable->GetImageName());
                                    else
                                        fprintf(stderr, "ERROR: couldn't get selected displayable's image!\n");
                                    // tu skala nie jest podana bezposrednio...
                                    mScale = displayable->GetSFSprite()->GetScale().x;
                                }
                                else
                                    fprintf(stderr, "ERROR: couldn't get SFSprite from displayable!\n");
                            }
                            mRotation = displayable->GetRotation();
                        }
                        else if (CPhysical* phys = mSelectedToErase->GetPhysical())
                        {
                            // physical?
                            if (dynamic_cast<CDynamicRegion*>(phys) != NULL)    // region?
                                SetSelected("region");
                            else    // inny physical?
                            {
                                if (phys->GetTemplate())
                                    SetSelected(phys->GetTemplate()->GetFilename());
                                else
                                    fprintf(stderr, "ERROR: couldn't get selected physical's template!\n");

                                // skala nie ma zastosowania w physicalach
                                mRotation = (float)phys->GetRotation();
                            }
                        }
                        else
                            fprintf(stderr, "ERROR: couldn't get selected displayable or physical!\n");
                    }
#endif 

				    map->EraseDoodah( mSelectedToErase->GetDoodahDescriptorId() );
				    map->EraseMapObject( mSelectedToErase->GetMapObjectDescriptorId() );
				}

				gScene.RemoveSceneNode(mSelectedToErase, true);
				mSelectedToErase = NULL;

                if (mMode == MODIFYING_MODE)
                    SetMode(PLACEMENT_MODE, mIsModifying);  // zachowaj aktualny stam mIsModifying
			}
		}
		else if (e.Button == sf::Mouse::Right){
			mMouseRightDown = true;
			mSavedMousePos.x = (float) e.X;
			mSavedMousePos.y = (float) e.Y;
			mSavedCameraPos = gCamera.GetPositionInGame();
		}
	}
}

void CEditor::MouseReleased( const sf::Event::MouseButtonEvent &e ){
	if (!mRunning) return;

    if (e.Button == sf::Mouse::Left)		mMouseLeftDown = false;
	else if (e.Button == sf::Mouse::Right)	mMouseRightDown = false;
}

void CEditor::MouseMoved( const sf::Event::MouseMoveEvent& e ){
	if (!mRunning) return;

	if (MouseInWorkspace())
	{
		if (mMode == PLACEMENT_MODE && mMouseLeftDown && mSelected && mSelected->ContinuousPlacement())
			mSelected->Place(GetMousePosInWorld(),mRotation,mScale,mZIndexFlag);
	}

	if (mMouseRightDown){
		sf::Vector2f mousePos( float(e.X), float(e.Y) );
		sf::Vector2f mouseMove = mousePos - mSavedMousePos;
		gCamera.GoTo( mSavedCameraPos - mouseMove/float(Map::TILE_SIZE), 0.0f );
	}
}

void CEditor::KeyPressed( const sf::Event::KeyEvent &e ){
	if (!mRunning) return;

	switch (e.Code) {
		case sf::Key::Up:		mScaleSpeed = 0.5f; break;
		case sf::Key::Down:		mScaleSpeed =-0.5f; break;
		case sf::Key::Left:		mRotationSpeed = 90.0f; break;
		case sf::Key::Right:	mRotationSpeed =-90.0f; break;
        case sf::Key::W:        { sf::View & view = gGame.GetRenderWindow()->GetDefaultView(); view.SetCenter(sf::Vector2f(0, -Map::TILE_SIZE) + view.GetCenter()); break; }
        case sf::Key::S:        { sf::View & view = gGame.GetRenderWindow()->GetDefaultView(); view.SetCenter(sf::Vector2f(0, +Map::TILE_SIZE) + view.GetCenter()); break; }
        case sf::Key::A:        { sf::View & view = gGame.GetRenderWindow()->GetDefaultView(); view.SetCenter(sf::Vector2f(-Map::TILE_SIZE, 0) + view.GetCenter()); break; }
        case sf::Key::D:        { sf::View & view = gGame.GetRenderWindow()->GetDefaultView(); view.SetCenter(sf::Vector2f(+Map::TILE_SIZE, 0) + view.GetCenter()); break; }
		case sf::Key::Z:		mZIndexFlag = !mZIndexFlag; break;
        case sf::Key::I:       
            mShowInvisibleWalls = !mShowInvisibleWalls;
            if (gMapManager.GetCurrent())
                gMapManager.GetCurrent()->ShowInvisibleWalls(mShowInvisibleWalls);
            break;
        case sf::Key::M:        SetMode(MODIFYING_MODE); break;
		default: break;
	}
}

void CEditor::KeyReleased( const sf::Event::KeyEvent &e ){
	if (!mRunning) return;

	switch (e.Code) {
		case sf::Key::Up:	case sf::Key::Down:		mScaleSpeed = 0.0f; break;
		case sf::Key::Left:	case sf::Key::Right:	mRotationSpeed = 0.0f; break;
		default: break;
	}
}

void CEditor::FrameStarted( float secondsPassed ){
    if (!mRunning) return;

	mScale = Maths::Clamp( mScale + mScaleSpeed*secondsPassed, 0.1f, 10.0f );
	mRotation = fmod( mRotation + mRotationSpeed*secondsPassed, 360.0f );

	if (mMode == PLACEMENT_MODE && mSelected)
	{
		sf::Vector2f mousePos = GetMousePosInWorld();
		mSelected->UpdateDisplayable(mPreviewer, mousePos, mRotation, mScale, mZIndexFlag);
    }
	else if (mMode == DOODAHS_ERASING_MODE)
		SetSelectedToErase( gScene.GetDisplayableAt(GetMousePosInWorld()) );
	else if (mMode == PHYSICALS_ERASING_MODE || mMode == REGIONS_ERASING_MODE)
    {
        CSceneNode* node = gScene.GetPhysicalAt(GetMousePosInWorld());
		SetSelectedToErase( node );
    }
    else if (mMode == MODIFYING_MODE)
    {
        // modyfikacja tyczy sie zarowno doodahow, jak i physicali
        // odsiewamy invisible-walle, jesli nie sa wyswietlane
        std::vector<CPhysical*> nodes;
	    gScene.GetPhysicalsInRadius(GetMousePosInWorld(), 0.02f, PHYSICAL_ANY, nodes);
        CSceneNode* node = NULL;

        for (std::vector<CPhysical*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
        {
            CPhysical* tmp = *it;

            // jesli znalezlismy invisible-walla, ale nie jest on widoczny, to olewamy
            if (!mShowInvisibleWalls &&
                tmp &&
                tmp->GetTemplate() &&
                tmp->GetTemplate()->GetFilename().find("invisible-wall") != std::string::npos)
                continue;

            node = tmp->GetSceneNode();
        }
        
        if (!node)
            node = gScene.GetDisplayableAt(GetMousePosInWorld());
        SetSelectedToErase( node );
    }

	if (mEditorScreens)
		mEditorScreens->Update();
}

bool CEditor::FramesDuringPause(EPauseVariant pv){
    return true;
}

void CEditor::Init(){
#ifndef __EDITOR__
    gResourceManager.ForceLoadAll();
    LoadTiles();
	LoadDoodahs();
	LoadPhysicals();
#endif

    mRunning = true;
    gGame.SetFreezed(true);
	gCamera.SetViewArea(mWorkspace.Left*100.0f, mWorkspace.Top*100.0f, mWorkspace.Right*100.0f, mWorkspace.Bottom*100.0f);
}

void CEditor::Stop(){
    mRunning = false;
    gGame.SetFreezed(false);
	gCamera.SetViewArea(0.0f,0.0f,100.0f,100.0f);
}

void CEditor::SetMode( CEditor::editorMode mode, bool modifying ){
    static std::string names[] = { "PLACEMENT_MODE", "DOODAHS_ERASING_MODE", "PHYSICALS_ERASING_MODE", "REGIONS_ERASING_MODE", "MODIFYING_MODE" };
    if (mMode != mode)
        fprintf(stderr, "Editor mode set to %s\n", names[(int)mode].c_str());

	mMode = mode;
    mIsModifying = modifying;

    // nie pokazujemy previewera, jesli zaznaczony jest region
    if (mSelected && dynamic_cast<CRegionResource*>(mSelected) == NULL)
	    mPreviewer->SetVisible( mode == PLACEMENT_MODE );
	SetSelectedToErase( mode == PLACEMENT_MODE ? NULL : mSelectedToErase );
}

CEditor::editorMode CEditor::GetMode() {
	return mMode;
}


void CEditor::LockMousePos() {
    mLockedMousePos = GetMousePosInWorld();
    mUseLockedMousePos = true;
}

sf::Vector2f CEditor::GetLockedMousePos()
{
    // edytor - ujednolicenie wspolrzednych z eventa wx'ow i edytora
    if (mUseLockedMousePos)
    {
        mUseLockedMousePos = false;
        return mLockedMousePos;
    }
    else
        return GetMousePosInWorld();
}

void CEditor::LoadTiles(){
    mTiles = new CEditorResourceSet();
    std::vector<std::string> tilePaths;
    gResourceManager.GetAllStartingWith("data/maps/themes","sf::Image",tilePaths);
	//warning: format '%u' expects type 'unsigned int', but argument 3 has type 'size_t'
    fprintf(stderr,"found %d tiles\n",(int)tilePaths.size());
    for (unsigned int i = 0; i < tilePaths.size(); i++)
        mTiles->Add(new CTileResource(tilePaths[i]));
}

void CEditor::LoadDoodahs(){
	mDoodahs = new CEditorResourceSet();
	std::vector<std::string> doodahPaths;
	gResourceManager.GetAllStartingWith("data/maps/doodahs/","sf::Image",doodahPaths);
	//warning: format '%u' expects type 'unsigned int', but argument 3 has type 'size_t'
	fprintf(stderr,"found %d doodahs\n",(int)doodahPaths.size());
	for (unsigned i = 0; i < doodahPaths.size(); i++)
		mDoodahs->Add(new CDoodahResource(doodahPaths[i]));
}

void CEditor::LoadPhysicals(){
	mPhysicals = new CEditorResourceSet();
	std::vector<std::string> physicalPaths;
	gResourceManager.GetAllStartingWith("data/physicals", "CDoorTemplate", physicalPaths);
	gResourceManager.GetAllStartingWith("data/physicals", "CEnemyTemplate", physicalPaths);
	gResourceManager.GetAllStartingWith("data/physicals", "CItemTemplate", physicalPaths);
	gResourceManager.GetAllStartingWith("data/physicals", "CLairTemplate", physicalPaths);
	gResourceManager.GetAllStartingWith("data/physicals", "CNPCTemplate", physicalPaths);
	gResourceManager.GetAllStartingWith("data/physicals", "CObstacleTemplate", physicalPaths);
	gResourceManager.GetAllStartingWith("data/physicals", "CWallTemplate", physicalPaths);
	gResourceManager.GetAllStartingWith("data/physicals", "CHookTemplate", physicalPaths);
	//warning: format '%u' expects type 'unsigned int', but argument 3 has type 'size_t'
	fprintf(stderr,"found %d physicals\n",(int)physicalPaths.size());
	for (unsigned i = 0; i < physicalPaths.size(); i++)
		mPhysicals->Add(new CPhysicalResource(physicalPaths[i]));
}

void CEditor::SetSelected( IEditorResource *res){
	mRotation = 0.0f;
	mScale = 1.0f;
	mZIndexFlag = false;
    mSelected = res;
    mPreviewer->SetColor(1.f, 1.f, 1.f);    // od-kolorowanie
    if (dynamic_cast<CRegionResource*>(res) == NULL)
        // moze niepotrzebne, ale jesli to nie region to mozna odswiezyc :)
        mPreviewer->SetVisible(true);
    if (res != NULL)
        res->PrepareDisplayable(mPreviewer);
}

#ifdef __EDITOR__
    void CEditor::SetSelected( const std::string& path )
    {
        IEditorResource* res = NULL;

        if (path.find("data/maps/themes/") != std::string::npos || path.find("data/maps/generated_tiles") != std::string::npos)
        {
            if (!mTiles)
                mTiles = new CEditorResourceSet();

            if (!mTiles->Contains(path))
                mTiles->Add(path, res = new CTileResource(path));
            else
                res = mTiles->Get(path);
        }
        else if (path.find("data/maps/doodahs/") != std::string::npos)
        {
            if (!mDoodahs)
                mDoodahs = new CEditorResourceSet();

            if (!mDoodahs->Contains(path))
                mDoodahs->Add(path, res = new CDoodahResource(path));
            else
                res = mDoodahs->Get(path);
        }
        else if (path.find("data/physicals/") != std::string::npos || path.find("data/loots/") != std::string::npos)
        {
            if (!mPhysicals)
                mPhysicals = new CEditorResourceSet();

            if (!mPhysicals->Contains(path))
                mPhysicals->Add(path, res = new CPhysicalResource(path));
            else
                res = mPhysicals->Get(path);
        }
        else if (path == "region")
        {
            if (!mRegion)
                mRegion = new CRegionResource();

            res = mRegion;

            // nie chcemy, zeby wyswietlalo sie cokolwiek innego = trzeba ukryc poprzednie zaznaczenie
            // wszystko przez to, ze region wyswietla sie przez CPrimitivesDrawer, nie 'normalnie'
            // jako CDisplayable
            mPreviewer->SetVisible(false);
        }

        if (res != NULL)
            SetSelected(res);
        else
            fprintf(stderr, "ERROR: couldn't select item: %s\n", path.c_str());
    }
#endif

void CEditor::SetSelectedToErase( CSceneNode* sn ){

	if (mSelectedToErase != sn)
	{
        CDynamicRegion* r = NULL;
        if (mSelectedToErase)
        {
            if (mSelectedToErase->GetPhysical() && (r = dynamic_cast<CDynamicRegion*>(mSelectedToErase->GetPhysical())))
                r->SetSelectedToErase(false);
            else
		        mSelectedToErase->SetPreviousColor();
        }

        if (sn)
        {
            if (sn->GetPhysical() && (r = dynamic_cast<CDynamicRegion*>(sn->GetPhysical())))
                r->SetSelectedToErase(true);
            else
                sn->SetColor(1.0f, 0.5f, 0.5f, 0.7f);
        }

		mSelectedToErase = sn;
	}
}

void CEditor::SetEditorWorkspace(float left, float top, float right, float bottom){
	mWorkspace.Left = left * 0.01f;
	mWorkspace.Top = top * 0.01f;
	mWorkspace.Right = right * 0.01f;
	mWorkspace.Bottom = bottom * 0.01f;
	gCamera.SetViewArea(left, top, right, bottom);
}

void CEditor::SetEditorScreens(GUI::CEditorScreens *es) {
	mEditorScreens = es;
}

bool CEditor::MouseInWorkspace(){
	const sf::Input& in = gGame.GetRenderWindow()->GetInput();
	sf::Vector2f mousePos(
		float(in.GetMouseX())/gGame.GetRenderWindow()->GetWidth(),
		float(in.GetMouseY())/gGame.GetRenderWindow()->GetHeight()
	);
	return mWorkspace.Contains(mousePos.x, mousePos.y);
}

sf::Vector2f CEditor::GetMousePosInWorld(){
    const sf::Input& in = gGame.GetRenderWindow()->GetInput();
    sf::Vector2f mousePos = gCamera.GetViewTopLeft() + sf::Vector2f((float)(in.GetMouseX()),(float)(in.GetMouseY()));
    mousePos.x /= Map::TILE_SIZE;
    mousePos.y /= Map::TILE_SIZE;
    return mousePos;
}
