#include "SAnimationFrame.h"


SAnimationFrame::SAnimationFrame( const std::string& _imageName, 
        const sf::IntRect& _rect,
        const sf::Vector2f& _offset,
        bool _isFlipped,
        const sf::Vector2i& _normalSize ):
    imageName( _imageName ),
    rect( _rect ),
    normalSize( _normalSize ),
    isFlipped( _isFlipped )
{
    if ( _offset.x != 0 && _offset.y != 0 )
    {
        if ( isFlipped )
        {
            offset.x = rect.Right - _offset.x;
            offset.y = _offset.y - rect.Top;
        }
        else
        {
            offset.x = _offset.x - rect.Left;
            offset.y = _offset.y - rect.Top;
        }
    }
    else
        offset = _offset;
}

