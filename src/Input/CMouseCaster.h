/* Takie fajne 'okienko' co sie wyswietla jak sie wcisnie PPM w sterowaniu MouseCast */

#ifndef MOUSE_CASTER_H
#define MOUSE_CASTER_H

#include "../GUI/CButton.h"
#include <string>
#include <vector>
#include <SFML/Graphics/Color.hpp>

namespace GUI { class CButton; }

enum mouseCasterState
{
	MC_CAST,
	MC_FOCUS,
	MC_IDLE
};

class CMouseCaster
{
public:
	CMouseCaster();
	~CMouseCaster();

    std::vector<std::wstring> GetAbiKeys();
    float GetRadius() { return mRadius; }
    void ResetGuiControls() { mKeys.clear(); }

	void Initialize( std::vector<std::wstring>& abiKeys, float radius );
	void SetKeySize( const sf::Vector2f& size, GUI::guiUnit u = GUI::UNIT_PIXEL );
	void SetKeyImage( const std::string& normal, const std::string& mouseOver="" );
	void SetKeyFont( const std::string& name, float size, GUI::guiUnit u = GUI::UNIT_PIXEL );
	void SetKeyColor( const sf::Color color );

	void Cast();
	void Focus();
	void Stop();

	void Update();

	inline bool IsVisible()		{ return mState == MC_CAST; }
	inline int GetLastKey()		{ return mLastKey; } // ZW=[0-keyCnt-1]
	inline int GetCurrentKey()	{ return mCurrentKey; }

	inline const sf::Vector2i& GetSavedCursorPosition() { return mCursorPos; }

private:
	mouseCasterState mState;
	std::vector<GUI::CButton*> mKeys;
	sf::Vector2f mKeySize[GUI::UNIT_COUNT];
	float mRadius;
	int mLastKey;
	int mCurrentKey;
	sf::Vector2i mCursorPos;

	void UpdatePosition();
	void SaveCursorPosition();
	void RestoreCursorPosition();
};

#endif