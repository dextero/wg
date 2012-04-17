#include "CLocalizator.h"
#include "CLocale.h"
#include "../CGUIObject.h"

#include <boost/filesystem.hpp>

#include <cstdarg>

template<> CLocalizator * CSingleton<CLocalizator>::msSingleton = 0;

const std::wstring emptyString = L"";
const std::string  emptyFont   =  "";

void CLocalizator::SetLocale(const std::string & localePrefix){
    Unload();

    std::string localeDir = "data/locale/";
    boost::filesystem::directory_iterator di(localeDir), di_end;
    
    // wszystko z data/locale
    for (; di != di_end; ++di)
    {
//        if (di->leaf() == ".svn") {
//            continue;
//        }
        std::string file = localeDir + di->leaf();
        if (!boost::filesystem::is_directory(file)) {
            gLocalizator.Load(file);
        }
    }
    // wszystko z data/locale/[jezyk], rekurencyjnie
    std::vector<std::string> dirs;
    dirs.push_back(localeDir + localePrefix);
    while (dirs.size() > 0)
    {
        std::string dir = dirs[dirs.size()-1];
        dirs.pop_back();
        boost::filesystem::directory_iterator di(dir),dir_end;
        for (; di != dir_end; di++){
//            if (di->leaf() == ".svn") {
//                continue;
//            }
            std::string file = dir + "/" + di->leaf();
            if (boost::filesystem::is_directory(file)) {
                dirs.push_back(file);
            }
            else
            {
                if (file.rfind(".xml") != file.length() - 4) {
                    fprintf(stderr, "Localizator: found some non .xml file in locale, %s\n", file.c_str());
                } else {
                    gLocalizator.Load(file);
                }
            }
        }
    }
    // koniec ladowania locale
}

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
