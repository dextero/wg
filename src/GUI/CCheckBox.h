/*	GUI - checkbox 
*/

#ifndef _CHECKBOX_H_
#define _CHECKBOX_H_

#include "CGUIObject.h"
#include <string>

class CHudSprite;

namespace GUI
{
	class CCheckBox : public CGUIObject
	{
	public:
		void SetImage( const std::string& imgTrue, const std::string& imgFalse );
		
		inline bool GetState()				 { return mChecked; }
		inline void SetState( bool checked ) { mChecked = checked; }

	private:
		CCheckBox( const std::string& name, CGUIObject* parent, unsigned zindex );
		virtual ~CCheckBox();
		virtual bool OnMouseEvent( float x, float y, mouseEvent e );
		virtual void UpdateSprites( float secondsPassed );

		CHudSprite*	mTrueSprite;
		CHudSprite* mFalseSprite;

		bool mChecked;

		friend class CGUIObject;
	};
};

#endif//_CHECKBOX_H_

