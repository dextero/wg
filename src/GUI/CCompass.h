#ifndef CCOMPASS_H
#define CCOMPASS_H

#include <SFML/System/Vector2.hpp>

#include <vector>

namespace GUI
{
    class CGUIObject;
    class CImageBox;

    class CCompass
    {
        public:
        CCompass();
        virtual ~CCompass();
        void Init();
        void Update( float dt );
		void Show();
		void Hide();
        void UpdatePosition();
        bool FindExit();
        void PointExit();
        void ClearExit();

        private:
        CGUIObject *mHud;
        sf::Vector2f mPos;
        CImageBox *mBackground;
        std::vector<CImageBox *> mArrows;
        float mRotation;
    };
}

#endif // CCOMPASS_H
