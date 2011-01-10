#ifndef __CEDITOR_H__
#define __CEDITOR_H__

#include "../Utils/CSingleton.h"
#include "../IFrameListener.h"
#include "../IMouseListener.h"
#include "../IKeyListener.h"

#include <set>
#include <string>
#include <SFML/System/Vector2.hpp> 
#include <SFML/Graphics/Rect.hpp>
 
#define gEditor CEditor::GetSingleton()

namespace GUI { class CEditorScreens; }
class CEditorResourceSet;
class IEditorResource;
class CDisplayable;
class CSceneNode;

class CEditor : public CSingleton< CEditor >, public IFrameListener, public IMouseListener, public IKeyListener {
public:

	enum editorMode {
		PLACEMENT_MODE,
		DOODAHS_ERASING_MODE,
		PHYSICALS_ERASING_MODE,
        REGIONS_ERASING_MODE,
        MODIFYING_MODE
	};

    CEditor();
    virtual ~CEditor();
    
	virtual void MousePressed( const sf::Event::MouseButtonEvent &e );
	virtual void MouseReleased( const sf::Event::MouseButtonEvent &e );
	virtual void MouseMoved( const sf::Event::MouseMoveEvent& e );
    
    virtual void KeyPressed( const sf::Event::KeyEvent &e );
    virtual void KeyReleased( const sf::Event::KeyEvent &e );
    
    virtual void FrameStarted( float secondsPassed );
    virtual bool FramesDuringPause(EPauseVariant pv);

    void Init();
    void Stop();

    inline CEditorResourceSet *GetTiles()		{ return mTiles; }
	inline CEditorResourceSet *GetDoodahs()		{ return mDoodahs; }
	inline CEditorResourceSet *GetPhysicals()	{ return mPhysicals; }

    inline IEditorResource* GetSelected()	{ return mSelected; }
    void SetSelected( IEditorResource *res);

#ifdef __EDITOR__
    void SetSelected( const std::string& path );
#endif

	void SetSelectedToErase( CSceneNode* sn );

	void SetEditorWorkspace(float left, float top, float right, float bottom);
	void SetEditorScreens(GUI::CEditorScreens* es);

	void SetMode( editorMode mode, bool modifying = false );
	editorMode GetMode();
private:
    bool mRunning;
	editorMode mMode;

	bool mMouseLeftDown;
	bool mMouseRightDown;
	sf::Vector2f mSavedMousePos;
	sf::Vector2f mSavedCameraPos;
    
    CEditorResourceSet *mTiles;
	CEditorResourceSet *mDoodahs;
	CEditorResourceSet *mPhysicals;
    IEditorResource    *mRegion;

    IEditorResource *mSelected;
	CSceneNode *mSelectedToErase;
    CDisplayable *mPreviewer;

	float mRotation;
	float mRotationSpeed;
	float mScale;
	float mScaleSpeed;
	bool  mZIndexFlag;
    bool  mShowInvisibleWalls;
    bool  mIsModifying;

    // do edytora - bo byla roznica w pozycji w evencie wx'ow i tu
    bool  mUseLockedMousePos;
    sf::Vector2f mLockedMousePos;
    void  LockMousePos();
    sf::Vector2f GetLockedMousePos();

	sf::Rect<float> mWorkspace; // obszar roboczy edytora - niezasloniety przez GUI [%]
	GUI::CEditorScreens* mEditorScreens;

	void LoadTiles();
	void LoadDoodahs();
	void LoadPhysicals();
	bool MouseInWorkspace();
    sf::Vector2f GetMousePosInWorld();

#ifdef __EDITOR__
    friend class CEditorWnd;
#endif
};


#endif//__CEDITOR_H__
