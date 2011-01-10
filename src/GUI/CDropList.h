/*	GUI - droplist
*/

#ifndef _DROPLIST_H_
#define _DROPLIST_H_

#include "CGUIObject.h"
#include "CButton.h"
#include <string>
#include <vector>
#include <SFML/Graphics/Color.hpp>

namespace GUI
{
	struct SOptionData
	{
		std::string normalImage, mouseOverImage;
		std::string font;
		float		fontSize[UNIT_COUNT];
		sf::Color	color;
		bool		center;

        SOptionData() : color(sf::Color::White), center(false)
        {
            for (unsigned int i = 0; i < UNIT_COUNT; ++i)
                fontSize[i] = 0.f;
        }
	};

	class CDropList : public CGUIObject
	{
	public:
		void SetImage( const std::string& normal, const std::string& mouseOver="" );
		void SetFont( const std::string& name, float size, guiUnit u = UNIT_PIXEL );
		void SetColor( const sf::Color color );
		void SetCenter( bool center );

		void SetOptionImage( const std::string& normal, const std::string& mouseOver="" );
		void SetOptionFont( const std::string& name, float size, guiUnit u = UNIT_PIXEL );
		void SetOptionColor( const sf::Color color );
		void SetOptionCenter( bool center );

		void AddOption( const std::wstring& option );
		void SetSelectedOption( const std::wstring& option );
		std::wstring GetSelectedOption();
        size_t GetSelectedIndex();

		void ShowList();
		void HideList();

	private:
		CDropList( const std::string& name, CGUIObject* parent, unsigned zindex );
		virtual ~CDropList();

		virtual bool OnMouseEvent( float x, float y, mouseEvent e );
		virtual void UpdateSprites( float secondsPassed );

		void PassLeftPaddingToChilds( guiUnit u );
		
		CButton * mSelectedOption;
		std::vector<CButton*> mOptions;
		SOptionData mOptionData;
		bool mShowList;

		friend class CGUIObject;
	};
};

#endif//_DROPLIST_H_

