/* klasy publiczne reprezentujace akcje wykonywane w czasie wyswietlania cutscenek 
   np. dŸwiêki, teksty, ruchy kamery etc */

#ifndef _CUTSCENE_ACTIONS_H_
#define _CUTSCENE_ACTIONS_H_

#include <string>
#include <SFML/System/Vector2.hpp>

namespace GUI
{
	enum caType
	{
		CUTSCENE_IMAGE,
		CUTSCENE_CAMERA,
		CUTSCENE_SOUND,
		CUTSCENE_TEXT
	};

	/* =================== */

	class CCutsceneAction 
	{	
	public:	
		float mTime;
		float mDuration; 
		caType mType;

        CCutsceneAction() :	mTime( 0.0f ), mDuration( 0.0f ), mType(CUTSCENE_IMAGE) {} // dex: nie mam pomyslu, co wsadzic jako default (mType)
		virtual ~CCutsceneAction() {}

		static bool Compare( CCutsceneAction* a, CCutsceneAction* b ) { return a->mTime > b->mTime; }
	};

	/* =================== */

	class CCutsceneImage : public CCutsceneAction
	{
	public:
		std::string mImagePath;
		CCutsceneImage() { mType = CUTSCENE_IMAGE; }
	};

	/* =================== */

	class CCutsceneCamera : public CCutsceneAction
	{
	public:
		sf::Vector2f mPosition;
		float mSize;
		CCutsceneCamera() : mPosition( 0.0f, 0.0f ), mSize( 0.0f ) { mType = CUTSCENE_CAMERA; }
	};

	/* =================== */

	class CCutsceneSound : public CCutsceneAction
	{
	public:
		std::string mSoundPath;
		CCutsceneSound() { mType = CUTSCENE_SOUND; }
	};

	/* =================== */

	class CCutsceneText : public CCutsceneAction
	{
	public:
		std::wstring mContent;
		CCutsceneText() { mType = CUTSCENE_TEXT; }
	};
};

#endif

