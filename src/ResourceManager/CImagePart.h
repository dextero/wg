#ifndef __CIMAGEPART_H__
#define __CIMAGEPART_H__

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace System{
    namespace Resource{
        class CImage;
    }
}

class CImagePart
{
private:
    System::Resource::CImage *mImage;
    sf::IntRect mOffset;
    
public:
    CImagePart() : mImage(NULL), mOffset(sf::IntRect(0,0,0,0)) {}
    CImagePart(System::Resource::CImage *img, sf::IntRect r) : mImage(img), mOffset(r) {}

    const System::Resource::CImage* GetImage() const { return mImage; }
    const sf::IntRect& GetOffset() const { return mOffset; }

    operator const System::Resource::CImage* () const { return mImage; }
    operator const sf::IntRect& () const { return mOffset; }
};

#endif //__CIMAGEPART_H__//
