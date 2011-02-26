#ifndef _CUTSCENE_PLAYER_H_
#define _CUTSCENE_PLAYER_H_

#include <string>
#include <vector>
#include "../../IFrameListener.h"
#include "../../IKeyListener.h"
#include "../../Utils/CSingleton.h"

#define gCutscenePlayer GUI::CCutscenePlayer::GetSingleton()

namespace GUI
{
	class CCutsceneAction;
	class CCutsceneImage;
	class CCutsceneCamera;
	class CCutsceneSound;
	class CCutsceneText;
	class CImageBox;
	class CTextArea;

	enum cutsceneState
	{
		CUTSCENE_EMPTY,
		CUTSCENE_PLAYING
	};
 
	class CCutscenePlayer : public IFrameListener, public IKeyListener, public CSingleton<CCutscenePlayer>
	{
	public:
		CCutscenePlayer();
		~CCutscenePlayer();

		void PlayCutscene( const std::string& cutscene );
		void StopCutscene();
		inline cutsceneState GetState()	{ return mState; }

		virtual void FrameStarted( float secondsPassed );
		virtual bool FramesDuringPause(EPauseVariant pv) { return true; };

		virtual void KeyPressed( const sf::Event::KeyEvent &e );

	private:
		std::vector<CCutsceneAction*> mActionsToExecute;
		std::vector<CCutsceneAction*> mActionsInProgress;
		
		float mTime;
		cutsceneState mState;
		CImageBox * mImageBox;
		CTextArea * mTextArea;
	};
};

#endif

