#include "CPhysical.h"
#include "Quests/CQuestManager.h"
#include "Quests/CQuestEvent.h"
#include "../Map/SceneManager/CSceneNode.h"
#include "../Map/CMapManager.h"
#include "../Rendering/CDisplayable.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/ZIndexVals.h"
#include "../Rendering/Animations/SAnimation.h"
#include "../Rendering/Animations/SAnimationState.h"
#include "../Rendering/Particles/CParticleManager.h"
#include "../Rendering/Effects/CGraphicalEffects.h"
#include "../Utils/Maths.h"

const float shadowAlpha = 0.25f;

CPhysical::CPhysical(const std::wstring& uniqueId) :
    mReadyForDestruction(false),
    mUseShadow(false),
	mUseDisplayable(true),
    mTemplate(NULL), 
    mUniqueId(uniqueId),
    mRotation(0),
	mCollisionShape( SHAPE_CIRCLE ),
    mCircleRadius(0.5f),
    mRectSize(1.0f,1.0f),
    mColorRed(1.0f),
    mColorGreen(1.0f),
    mColorBlue(1.0f),
    mColorAlpha(1.0f),
    mShadowOffset(0.1f),
    myZIndex(Z_PHYSICAL),
    mPhysicalManagerIndex(-1),
    mDisplayable( NULL ),
    mShadow( NULL ),
    mSceneNode( NULL ),
    mSummonContainerPtr(NULL)
{
	mSceneNode = new CSceneNode();
	mSceneNode->SetPhysical( this );
    gScene.AddSceneNode( mSceneNode );
}

CPhysical::~CPhysical() {

	if ( mSceneNode ) {
		gScene.RemoveSceneNode( mSceneNode );
        delete mSceneNode;
	}

	if ( mDisplayable )
		gDrawableManager.DestroyDrawable( mDisplayable );

    if ( mShadow )
        gDrawableManager.DestroyDrawable( mShadow );

    part::gParticleManager.RemoveFromPhysical(this);
    gGraphicalEffects.NoticePhysicalDestroyed(this);

	if ( mUniqueId != L"" )
		gQuestManager.HandleEvent( new Quests::CPhysicalDeathById( mUniqueId ) );

	if ( mGenre != L"" )
		gQuestManager.HandleEvent( new Quests::CPhysicalDeathByGenre( mGenre ) );
}

void CPhysical::MarkForDelete(){
    mReadyForDestruction = true;
}

void CPhysical::Kill(){
    MarkForDelete();
}

void CPhysical::Update(float dt) {
    sf::Vector2f newPosition;
    newPosition.x = mPosition.x + ( dt * mVelocity.x );
    newPosition.y = mPosition.y + ( dt * mVelocity.y );

    mOldPosition = mPosition;
    this->SetPosition( newPosition );
}

const std::wstring& CPhysical::GetUniqueId() const {
    return mUniqueId;
}

void CPhysical::SetGenre( const std::wstring& g ) { 
	mGenre = g; 
}

const std::wstring& CPhysical::GetGenre() { 
	return mGenre; 
}

physCategory CPhysical::GetCategory() {
    return mCategory;
}

void CPhysical::SetCategory( physCategory newValue ) {
    mCategory = newValue;
}

void CPhysical::SetCategory( const std::wstring& newValue ) {
	if ( newValue == L"detector" )
		mCategory = PHYSICAL_DETECTOR;
	else if ( newValue == L"player" )
        mCategory = PHYSICAL_PLAYER;
    else if ( newValue == L"monster" )
        mCategory = PHYSICAL_MONSTER;
    else if ( newValue == L"bullet" )
        mCategory = PHYSICAL_BULLET;
    else if ( newValue == L"lair" )
        mCategory = PHYSICAL_LAIR;
    else if ( newValue == L"item" )
        mCategory = PHYSICAL_ITEM;
    else 
        fprintf(stderr,"unrecognised physical category: %ls for physical %ls\n",newValue.c_str(),GetUniqueId().c_str());
}

const sf::Vector2f& CPhysical::GetVelocity() const {
    return mVelocity;
}

void CPhysical::SetVelocity(const sf::Vector2f& newValue) {
    mVelocity = newValue;
}

const sf::Vector2f& CPhysical::GetOldPosition() const
{
    return mOldPosition;
}

const sf::Vector2f& CPhysical::GetPosition() const
{
    return mPosition;
}

void CPhysical::SetPosition(const sf::Vector2f& newValue, bool changeOldPosition) {
    mPosition = newValue;
    if (mDisplayable)
       mDisplayable->SetPosition( mPosition );
    if (mShadow)
        mShadow->SetPosition( mPosition.x + mShadowOffset, mPosition.y + mShadowOffset);
    if (changeOldPosition)
       mOldPosition = mPosition;
}

const std::string& CPhysical::GetImage() const {
    return mImage;
}

void CPhysical::SetImage(const std::string& newValue) {
    mImage = newValue;

    CheckDisplayable();

    if (mDisplayable)
        mDisplayable->SetStaticImage(mImage);
    if (mShadow)
        mShadow->SetStaticImage(mImage);
}

collisionShape CPhysical::GetCollisionShape()
{
	return mCollisionShape;
}

void CPhysical::SetBoundingCircle( float radius )
{
	mCollisionShape = SHAPE_CIRCLE;
	mCircleRadius = radius;
	mRectSize.x = mRectSize.y = radius * 2.0f;
}

float CPhysical::GetCircleRadius()
{
	return mCircleRadius;
}

void CPhysical::SetBoundingRect( const sf::Vector2f& size )
{
	mCollisionShape = SHAPE_RECT;
	mRectSize = size;
	mCircleRadius = Maths::Length( size ) * 0.5f;
}

const sf::Vector2f& CPhysical::GetRectSize() const
{
	return mRectSize;
}

const std::string& CPhysical::GetAnimationName() const
{
    static std::string empty_string("");
    if (mAnimation!=NULL)
        return mAnimation->name;
    else
        return empty_string;
}

SAnimation *CPhysical::GetAnimation(){
    return mAnimation;
}

int CPhysical::GetRotation()
{
    return mRotation;
}

void CPhysical::SetRotation(int newValue)
{
    mRotation=newValue;
    if (mDisplayable)
		mDisplayable->SetRotation((float)newValue);
    if (mShadow)
        mShadow->SetRotation((float)newValue);
}

void CPhysical::CheckDisplayable(){
    if ( (mUseDisplayable) && (!mDisplayable) )
    {
		
        mDisplayable = gDrawableManager.CreateDisplayable( myZIndex );
        if ( !mDisplayable )
        {
            fprintf( stderr, "error: CPhysical::CheckDisplayable, mDisplayable null, returning\n" );
            return;
        }
        mDisplayable->SetPosition( mPosition );
        mDisplayable->SetRotation( (float)mRotation );
        mDisplayable->SetColor( mColorRed, mColorGreen, mColorBlue, mColorAlpha );
    }
    if ((mUseShadow) && (!mShadow)){
        mShadow = gDrawableManager.CreateDisplayable( Z_SHADOWS );
        if ( !mShadow )
        {
            fprintf( stderr, "error: CPhysical::CheckDisplayable, mShadow null, returning\n" );
            return;
        }
        mShadow->SetPosition(mPosition.x + mShadowOffset, mPosition.y + mShadowOffset);
        mShadow->SetRotation((float)mRotation);
        mShadow->SetColor( 0.0f, 0.0f, 0.0f, shadowAlpha );
    }
}

void CPhysical::SetAnimation( const std::string& newValue )
{
    CheckDisplayable();

	if (mDisplayable)
		mDisplayable->SetAnimation( mAnimation );
    mAnimation = mDisplayable->GetAnimationState()->animation;
    if (mShadow)
        mShadow->SetAnimation(newValue);
}

void CPhysical::SetAnimation( SAnimation *anim )
{
    CheckDisplayable();
    
    mAnimation = anim;
	if (mDisplayable)
		mDisplayable->SetAnimation( anim );
    if (mShadow)
        mShadow->SetAnimation(anim);
}

CSceneNode* CPhysical::GetSceneNode()
{
    return mSceneNode;
}

CDisplayable * CPhysical::GetDisplayable()
{
	return mDisplayable;
}

CDisplayable * CPhysical::GetShadow()
{
	return mShadow;
}

void CPhysical::SetZIndex(int z){
    if ((myZIndex != z) &&(mDisplayable!= NULL))
        gDrawableManager.RegisterDrawable(mDisplayable,z);
    myZIndex = z;
}

CStats *CPhysical::GetStats(){
    return NULL;
}

CAppliedEffectContainer *CPhysical::GetAppliedEffectContainer(){
    return NULL;
}

CBleeder *CPhysical::GetBleeder(){
    return NULL;
}

void CPhysical::SetColor( float red, float green, float blue, float alpha )
{
    mColorRed = red;
    mColorGreen = green;
    mColorBlue = blue;
    mColorAlpha = alpha;

    if ( mDisplayable )
        mDisplayable->SetColor( mColorRed, mColorGreen, mColorBlue, mColorAlpha );
}

void CPhysical::GetColor( float * outRed, float * outGreen, float * outBlue, float * outAlpha )
{
    *outRed = mColorRed;
    *outGreen = mColorGreen;
    *outBlue = mColorBlue;
    *outAlpha = mColorAlpha;
}

float CPhysical::GetShadowOffset(){
    return mShadowOffset;
}

void CPhysical::SetShadowOffset(float shadowOffset){
    mShadowOffset = shadowOffset;
}

bool CPhysical::IsDead(){
    return mReadyForDestruction;
}
