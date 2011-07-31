#ifndef CPHYSICAL_H_
#define CPHYSICAL_H_

// klasa reprezentujaca obiekty w grze

#include <SFML/System/Vector2.hpp>
#include <string>

#include "PhysicalCategories.h"

enum collisionShape
{
	SHAPE_CIRCLE,
	SHAPE_RECT
};

class CSceneNode;
class CDisplayable;
struct SAnimation;
class CStats;
class CAppliedEffectContainer;
class CBleeder;
class CSummonContainer;
class CPhysicalTemplate;

class CPhysical
{
protected:
    friend class CPhysicalManager;
    
    /* konstruktor prywatny, uzywany wylacznie przez CPhysicalManager
     * by uzyskac obiekt CPhysical, wywolywac CPhysicalManager::CreatePhysical();
     */
    CPhysical( const std::wstring& uniqueId );
    CPhysical(CPhysical &other);
    virtual ~CPhysical();

    bool mReadyForDestruction;
    bool mUseShadow;
	bool mUseDisplayable;

	CPhysicalTemplate *mTemplate;
public:
    virtual void Update( float dt );

    const std::wstring& GetUniqueId() const;

	void SetGenre( const std::wstring& g );
	const std::wstring& GetGenre();

    SideAndCategory GetSideAndCategory() const;
    void SetCategory( physCategory newValue );
    void SetCategory( const std::wstring& newValue );
	void SetSide( ESide side);

    const sf::Vector2f& GetOldPosition() const;

    const sf::Vector2f& GetPosition() const;
    void SetPosition( const sf::Vector2f& newValue, bool changeOldPosition = false );
    
    const sf::Vector2f& GetVelocity() const;
    void SetVelocity( const sf::Vector2f& newValue );

    const std::string& GetImage() const;
    void SetImage( const std::string& newValue );

    const std::string& GetAnimationName() const;
    SAnimation *GetAnimation();
    void SetAnimation( const std::string& newValue );
    void SetAnimation( SAnimation *anim );

	collisionShape GetCollisionShape();
	virtual void SetBoundingCircle( float radius );
	float GetCircleRadius();
	void SetBoundingRect( const sf::Vector2f& size );
	const sf::Vector2f& GetRectSize() const;

    int GetRotation();                                // W stopniach 
    void SetRotation( int newValue );

    CSceneNode * GetSceneNode();
	CDisplayable * GetDisplayable();
    CDisplayable * GetShadow();

    void MarkForDelete();
    virtual void Kill();

    virtual CStats *GetStats();
    virtual CAppliedEffectContainer *GetAppliedEffectContainer();
    virtual CBleeder *GetBleeder();

    void SetColor( float red, float green, float blue, float alpha );
    void GetColor( float * outRed, float * outGreen, float * outBlue, float * outAlpha );

    float GetShadowOffset();
    void SetShadowOffset(float shadowOffset);
protected:
    physCategory mCategory;
	ESide mSide;
    std::wstring mUniqueId;
	std::wstring mGenre;
    sf::Vector2f mOldPosition;
    sf::Vector2f mPosition;
    sf::Vector2f mVelocity;
    std::string mImage;
    SAnimation *mAnimation;
    int mRotation;

	collisionShape mCollisionShape;
	float mCircleRadius;
	sf::Vector2f mRectSize;

    void SetZIndex(int z);

    float mColorRed, mColorGreen, mColorBlue, mColorAlpha;
    float mShadowOffset;
private:
    int myZIndex;
    int mPhysicalManagerIndex;

    void CheckDisplayable();
	CDisplayable * mDisplayable;
    CDisplayable * mShadow;
    CSceneNode * mSceneNode;
protected:
    CSummonContainer *mSummonContainerPtr;
public:
    inline CSummonContainer *GetSummonContainer(){ return mSummonContainerPtr; }
    virtual bool IsDead();

	inline void SetUseDisplayable(bool b){ mUseDisplayable = b; }

	inline CPhysicalTemplate *GetTemplate() const{
		return mTemplate;
	}
	inline void SetTemplate(CPhysicalTemplate *templ){
		mTemplate = templ;
	}

#ifdef __EDITOR__
    friend class CEditorWnd;
#endif
};

#endif /* CPHYSICAL_H_ */
