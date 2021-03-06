#ifndef _CLOCALIZATOR_H_
#define _CLOCALIZATOR_H_
// globalny mechanizm tlumaczacy klucze -> stringi

#include <string>
#include "KeyMacros.h"
#include "../../Utils/CSingleton.h"
#include "CLocale.h"

#define gLocalizator CLocalizator::GetSingleton()

class CLocalizator : public CSingleton<CLocalizator> {
private:
	CLocale	*mCurrent;
public:
    void SetLocale(const std::string & localePrefix);

	void Load(const std::string &filename);
    void Unload() { if (mCurrent) delete mCurrent; mCurrent = NULL; }

    const SLocalizedText Get(hash_t id);
    const SLocalizedText Get(const char* id);

	const std::wstring & GetText(hash_t id);
    const std::wstring & GetText(const char* id); // dex: visual ma problem z std::string, chce KONIECZNIE
                                            // konwertowac char* na hash_t, zamiast std::string.
    STextPadding GetPadding(hash_t id);
    STextPadding GetPadding(const char* id);
    
    const std::wstring Localize(std::string str);
    const std::wstring Localize(std::wstring str);

    const std::wstring GetTextf(const char* id, ...);

	CLocalizator(): mCurrent(NULL){}
};

#endif// _CLOCALIZATOR_H_

