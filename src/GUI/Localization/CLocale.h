#ifndef _CLOCALE_H_
#define _CLOCALE_H_

#include "KeyMacros.h"
#include <string>
#include <map>

namespace GUI {
    enum guiUnit
	{
		UNIT_PERCENT = 0,
		UNIT_PIXEL
	};

	const unsigned int UNIT_COUNT = 2;
};

struct STextPadding {
    union {
        struct {
            float left, top, right, bottom;
        };
        float padding[4];
    };

    STextPadding(float left = 0.f, float top = 0.f, float right = 0.f, float bottom = 0.f);
    STextPadding(std::wstring& str);
};

struct SLocalizedText {
    std::wstring text;
    STextPadding padding;

    SLocalizedText(std::wstring text = L"-undefined-",
                   STextPadding padding = STextPadding());
};

class CLocale{
private:
	std::map<hash_t, SLocalizedText> mKeys;
	std::wstring mName;
	std::wstring mCode;
public:
    const SLocalizedText Get(hash_t id);
	const std::wstring &GetText(hash_t id);
    STextPadding GetPadding(hash_t id);

    const std::wstring& GetCode() { return mCode; }
	
	void Load(std::string filename);

	CLocale();
	~CLocale();
};

#endif// _CLOCALE_H_

