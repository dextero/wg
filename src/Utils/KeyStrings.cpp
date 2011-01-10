#include "KeyStrings.h"
#include <SFML/Window/Event.hpp>
#include "../GUI/Localization/CLocalizator.h"
#include <sstream>

struct SKeyString
{
	int key;
	std::string str;
};

static SKeyString keysArray[] =
{
	{	sf::Key::A,		"A" },
	{	sf::Key::B,		"B" },
	{	sf::Key::C,		"C" },
	{	sf::Key::D,		"D" },
	{	sf::Key::E,		"E" },
	{	sf::Key::F,		"F" },
	{	sf::Key::G,		"G" },
	{	sf::Key::H,		"H" },
	{	sf::Key::I,		"I" },
	{	sf::Key::J,		"J" },
	{	sf::Key::K,		"K" },
	{	sf::Key::L,		"L" },
	{	sf::Key::M,		"M" },
	{	sf::Key::N,		"N" },
	{	sf::Key::O,		"O" },
	{	sf::Key::P,		"P" },
	{	sf::Key::Q,		"Q" },
	{	sf::Key::R,		"R" },
	{	sf::Key::S,		"S" },
	{	sf::Key::T,		"T" },
	{	sf::Key::U,		"U" },
	{	sf::Key::V,		"V" },
	{	sf::Key::W,		"W" },
	{	sf::Key::X,		"X" },
	{	sf::Key::Y,		"Y" },
	{	sf::Key::Z,		"Z" },

	{	sf::Key::Num0,	"0" },
	{	sf::Key::Num1,	"1" },
	{	sf::Key::Num2,	"2" },
	{	sf::Key::Num3,	"3" },
	{	sf::Key::Num4,	"4" },
	{	sf::Key::Num5,	"5" },
	{	sf::Key::Num6,	"6" },
	{	sf::Key::Num7,	"7" },
	{	sf::Key::Num8,	"8" },
	{	sf::Key::Num9,	"9" },	

	{	sf::Key::LControl,	"LCtrl" },
	{	sf::Key::LShift,	"LShift" },
	{	sf::Key::LAlt,		"LAlt" },
	{	sf::Key::RControl,	"RCtrl" },
	{	sf::Key::RShift,	"RShift" },
	{	sf::Key::RAlt,		"RAlt" },
	{	sf::Key::LBracket,	"[" },
	{	sf::Key::RBracket,	"]" },
	{	sf::Key::SemiColon,	";" },
	{	sf::Key::Comma,		"," },
	{	sf::Key::Period,	"." },
	{	sf::Key::Quote,		"'" },
	{	sf::Key::Slash,		"/" },
	{	sf::Key::BackSlash,	"\\" },
	{	sf::Key::Tilde,		"~" },
	{	sf::Key::Equal,		"=" },
	{	sf::Key::Dash,		"_" },
	{	sf::Key::Space,		"Space" },
	{	sf::Key::Return,	"Enter" },
	{	sf::Key::Back,		"Bkspc" },
	{	sf::Key::Tab,		"Tab" },
	{	sf::Key::PageUp,	"PgUp" },
	{	sf::Key::PageDown,	"PgDn" },
	{	sf::Key::End,		"End" },
	{	sf::Key::Home,		"Home" },
	{	sf::Key::Insert,	"Ins" },
	{	sf::Key::Delete,	"Del" },

	{	sf::Key::Add,		"Num+" },
	{	sf::Key::Subtract,	"Num-" },
	{	sf::Key::Multiply,	"Num*" },
	{	sf::Key::Divide,	"Num/" },

	{	sf::Key::Left,		"Left" },
	{	sf::Key::Right,		"Right" },
	{	sf::Key::Up,		"Up" },
	{	sf::Key::Down,		"Down" },

	{	sf::Key::Numpad0,	"Num0" },
	{	sf::Key::Numpad1,	"Num1" },
	{	sf::Key::Numpad2,	"Num2" },
	{	sf::Key::Numpad3,	"Num3" },
	{	sf::Key::Numpad4,	"Num4" },
	{	sf::Key::Numpad5,	"Num5" },
	{	sf::Key::Numpad6,	"Num6" },
	{	sf::Key::Numpad7,	"Num7" },
	{	sf::Key::Numpad8,	"Num8" },
	{	sf::Key::Numpad9,	"Num9" },

    {   sf::Key::F1,    "F1" },
    {   sf::Key::F2,    "F2" },
    {   sf::Key::F3,    "F3" },
    {   sf::Key::F4,    "F4" },
    {   sf::Key::F5,    "F5" },
    {   sf::Key::F6,    "F6" },
    {   sf::Key::F7,    "F7" },
    {   sf::Key::F8,    "F8" },
    {   sf::Key::F9,    "F9" },
    {   sf::Key::F10,    "F10" },
    {   sf::Key::F11,    "F11" },
    {   sf::Key::F12,    "F12" },

	{	sf::Key::Count + sf::Mouse::Left,	"LMB" },
	{	sf::Key::Count + sf::Mouse::Right,	"RMB" },
    {   sf::Key::Count + sf::Mouse::Middle, "MMB" },
    {   sf::Key::Count + sf::Mouse::XButton1,   "XMB1" },
    {   sf::Key::Count + sf::Mouse::XButton2,   "XMB2" },

	{   -1,					"" }
};

std::string KeyStrings::KeyToString( int key )
{
	for ( unsigned i = 0; keysArray[i].key != -1; i++ )
		if ( keysArray[i].key == key )
			return keysArray[i].str;
	return "";
}

int KeyStrings::StringToKey( const std::string& s )
{
	for ( unsigned i = 0; keysArray[i].key != -1; i++ )
		if ( keysArray[i].str == s )
			return keysArray[i].key;
	return -1;
}

const std::wstring KeyStrings::GetBindDesc( const std::string& s)
{
    std::string str;
    std::string::const_iterator i = s.begin();
    std::string::const_iterator end = s.end();

    // zamiana na wielkie litery
    while (i != end) {
        str += (*i >= 'a' && *i <= 'z' ? *i - 'a' + 'A' : *i);
        ++i;
    }

    std::stringstream ss;
    ss << "CTRL_" << str << "_DESC";
    return gLocalizator.GetText(ss.str().c_str());
}

