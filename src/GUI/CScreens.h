#ifndef CScreens_h__
#define CScreens_h__

#include <string>

namespace GUI
{
    class CScreens
    {
    public:
        virtual ~CScreens() {};

        virtual void Show( const std::wstring& menu ) = 0;
        virtual void ShowPrevious() = 0;
    };
}

#endif // CScreens_h__