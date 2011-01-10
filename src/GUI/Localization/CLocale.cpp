#include "CLocale.h"
#include "../../Utils/CXml.h"

#include "GeneralKeys.h"
#include "../CGUIObject.h"

#define LOAD_FONT(font, key, default) \
    if ((mFonts[font] = StringUtils::ConvertToString(GetText(StringUtils::GetStringHash(key)))) == "???") \
        mFonts[font] = default

STextPadding::STextPadding(float left, float top, float right, float bottom): left(left), top(top), right(right), bottom(bottom)
{
}

STextPadding::STextPadding(std::wstring& str) {
    unsigned int at = 0;
    for (unsigned int i = 0; i < 4; ++i, at = str.find(L",", at))
        if (at != std::wstring::npos)
            padding[i] = StringUtils::Parse<float>(str.substr(at, str.find(L",", at)));
};

SLocalizedText::SLocalizedText(std::wstring text, float fontSize, GUI::guiUnit fontSizeType, STextPadding padding):
    text(text),
    fontSize(fontSize),
    fontSizeType(fontSizeType),
    padding(padding)
{
}

// -------------------

const std::wstring notFound = L"???";

const SLocalizedText CLocale::Get(hash_t id){
	if (mKeys.count(id) > 0)
		return mKeys[id];
	else
        return SLocalizedText();
}

const std::wstring &CLocale::GetText(hash_t id){
	if (mKeys.count(id) > 0)
		return mKeys[id].text;
	else
		return notFound;
}

STextPadding CLocale::GetPadding(hash_t id){
    if (mKeys.count(id) > 0)
        return mKeys[id].padding;
    else
        return STextPadding();
}

float CLocale::GetFontSize(hash_t id){
    if (mKeys.count(id) > 0)
        return mKeys[id].fontSize;
    else
        return 100.f;
}

GUI::guiUnit CLocale::GetFontSizeType(hash_t id){
    if (mKeys.count(id) > 0)
        return mKeys[id].fontSizeType;
    else
        return GUI::UNIT_PIXEL;
}

const std::string& CLocale::GetFont(GUI::localFontType type)
{
    return mFonts[type];
}

void CLocale::Load(std::string filename){
	CXml xml(filename, "root");
	if (xml.GetString(xml.GetRootNode(),"type") != "locale")
		return;

    size_t oldKeys = mKeys.size();

	mName = StringUtils::ReinterpretFromUTF8(xml.GetString(xml.GetRootNode(),"name"));
	mCode = StringUtils::ReinterpretFromUTF8(xml.GetString(xml.GetRootNode(),"code"));

	for (rapidxml::xml_node<> *n = xml.GetChild(xml.GetRootNode(),"entry"); n; n = xml.GetSibl(n,"entry")){
		hash_t hash = StringUtils::GetStringHash(xml.GetString(xml.GetChild(n,"key")));
		std::wstring str = std::wstring(StringUtils::ReinterpretFromUTF8(xml.GetString(xml.GetChild(n,"value"))));
        std::wstring padding = StringUtils::ReinterpretFromUTF8(xml.GetString(xml.GetChild(n, "padding")));

        std::wstring sizeStr = StringUtils::ReinterpretFromUTF8(xml.GetString(xml.GetChild(n, "size")));
        GUI::guiUnit sizeType = GUI::UNIT_PIXEL;
        float size = 100.f;
        if (sizeStr != L"")
        {
            sizeType = (sizeStr[sizeStr.size() - 1] == L'%' ? GUI::UNIT_PERCENT : GUI::UNIT_PIXEL);
            size = StringUtils::Parse<float>(sizeStr);
        }

        if (padding != L"")
            mKeys[hash] = SLocalizedText(str, size, sizeType, STextPadding(padding));
        else
		    mKeys[hash] = SLocalizedText(str, size, sizeType);
	}

    fprintf(stderr, "Locale: %d keys in %s\n", (int)(mKeys.size() - oldKeys), filename.c_str());

    // szybszy dostep do fontow
    // dex: uwaga, wszystkie sciezki do fontow musza byc poprawne lub nie istniec
    LOAD_FONT(GUI::FONT_MENU, "FONT_MENU", "data/GUI/argbrujs.ttf");
    LOAD_FONT(GUI::FONT_DIALOG, "FONT_DIALOG", "data/GUI/verdana.ttf");
    LOAD_FONT(GUI::FONT_MESSAGE, "FONT_MESSAGE", "data/GUI/monotype-corsiva.ttf");
    LOAD_FONT(GUI::FONT_CONSOLE, "FONT_CONSOLE", "data/GUI/free_mono.ttf");
}

CLocale::CLocale(){
    for (unsigned int i = 0; i < GUI::FONT_COUNT; ++i)
        mFonts[i] = "";
}

CLocale::~CLocale(){
	//std::map<hash_t, SLocalizedText>::iterator it;
	//for (it = mKeys.begin(); it != mKeys.end(); it++)
	//	delete it->second.text;
}

