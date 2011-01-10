#include "CLocalizator.h"
#include "CLocale.h"
#include "../CGUIObject.h"

#include <cstdarg>

template<> CLocalizator * CSingleton<CLocalizator>::msSingleton = 0;

const std::wstring emptyString = L"";
const std::string  emptyFont   =  "";

void CLocalizator::Load(const std::string &filename){
	if (!mCurrent)
	    mCurrent = new CLocale();
	mCurrent->Load(filename);
}

const SLocalizedText CLocalizator::Get(hash_t id){
    if (mCurrent)
        return mCurrent->Get(id);
    else
        return SLocalizedText();
}

const SLocalizedText CLocalizator::Get(const char* id){
    if (mCurrent)
        return mCurrent->Get(StringUtils::GetStringHash(id));
    else
        return SLocalizedText();
}
	
const std::wstring & CLocalizator::GetText(hash_t id){
	if (mCurrent)
		return mCurrent->GetText(id);
	else
		return emptyString;
}

const std::wstring & CLocalizator::GetText(const char* id){
    if (mCurrent)
        return mCurrent->GetText(StringUtils::GetStringHash(id));
    else
        return emptyString;
}

STextPadding CLocalizator::GetPadding(hash_t id){
    if (mCurrent)
        return mCurrent->GetPadding(id);
    else
        return STextPadding();
}

STextPadding CLocalizator::GetPadding(const char* id){
    if (mCurrent)
        return mCurrent->GetPadding(StringUtils::GetStringHash(id));
    else
        return STextPadding();
}

float CLocalizator::GetFontSize(hash_t id){
    if (mCurrent)
        return mCurrent->GetFontSize(id);
    else
        return 100.f;
}

float CLocalizator::GetFontSize(const char* id){
    if (mCurrent)
        return mCurrent->GetFontSize(StringUtils::GetStringHash(id));
    else
        return 100.f;
}

GUI::guiUnit CLocalizator::GetFontSizeType(hash_t id){
    if (mCurrent)
        return mCurrent->GetFontSizeType(id);
    else
        return GUI::UNIT_PIXEL;
}

GUI::guiUnit CLocalizator::GetFontSizeType(const char* id){
    if (mCurrent)
        return mCurrent->GetFontSizeType(StringUtils::GetStringHash(id));
    else
        return GUI::UNIT_PIXEL;
}

const std::string& CLocalizator::GetFont(GUI::localFontType type)
{
    if (mCurrent)
        return mCurrent->GetFont(type);
    else
        return emptyFont;
}


const std::wstring CLocalizator::Localize(std::string str)
{
    return (str[0] == '$' ? GetText(str.substr(1).c_str()) : StringUtils::ReinterpretFromUTF8(str));
}

const std::wstring CLocalizator::Localize(std::wstring str)
{
    return (str[0] == L'$' ? GetText(StringUtils::ConvertToString(str.substr(1)).c_str()) : str);
}

const std::wstring CLocalizator::GetTextf(const char* id, ...)
{
    std::wstring out = L"";
    std::wstring txt = mCurrent->GetText(StringUtils::GetStringHash(id));
    wchar_t* text = new wchar_t[txt.size() + 1];
    wcscpy(text, txt.c_str());  // 100% konczy sie na \0

    static wchar_t s_Buffer[32768];
    if( wcslen(text)!=0 )
    {
        va_list args;
        va_start(args, id);
        vswprintf( s_Buffer, 32767, text, args );
        va_end(args);
        out += s_Buffer;
    }

    delete[] text;
    return out;
}