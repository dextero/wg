#include "EAspect.h"
#include <cstdio>
#include <cassert>
#include <cstring>

using namespace Aspects;

const std::wstring invString = L"inv";
const std::wstring logString = L"log";

EAspect parseAspect(const std::wstring &s){
    for (int i = 1; i < aspectCount; i++) // ignorujemy aNoAspect
        if (aspectNames[i]==s){
            int l = 1<<(i-1);
            assert((l>=0) && (l<=(1<<(aspectCount-1))));
            return EAspect(l);
        }
    if (s != L"")
        fprintf(stderr,"Failed to parse aspect: %ls. Returning noAspect\n",s.c_str());
    return aNoAspect;
}

int parseModifiers(const std::wstring &input){
    unsigned int char_cnt = (unsigned int)(input.size()+1);
    wchar_t *buf = new wchar_t[char_cnt];
    memcpy(buf,input.c_str(),(input.size()+1)*sizeof(wchar_t));
    for (unsigned int i = 0; i < char_cnt; i++){
        if (buf[i] == ',')
            buf[i] = '\0';
    }
    unsigned int offset = 0;
    int result = 0;
    while (offset < char_cnt){
        std::wstring code(buf+offset);
        offset+=(unsigned int)wcslen(buf+offset)+1;
		if (code == invString)
			result = result | invValue;
		else if (code == logString)
			result = result | logValue;
		else
			result = result | parseAspect(code);
    }
    delete[] buf;
    return result;
}

std::wstring availableAspects(){
    std::wstring s = L"";
    for (int i = 1; i < aspectCount; i++){ // ignorujemy aNoAspect
        if (i != 1) s+=L',';
        s+=aspectNames[i];
    }
    return s;
}

std::wstring aspectName(EAspect a){
	for (int i = 1; i < aspectCount; i++)
		if ((1 << (i-1)) == (int)a)
			return Aspects::aspectNames[i]; 
	return L"unknown-aspect";
}
