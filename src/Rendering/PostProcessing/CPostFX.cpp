#include "CPostFX.h"
#include "../../CGame.h"
#include "../CCamera.h"

sf::Image &pfx::CPostFX::GetScreen()
{
    static sf::Image screen;
    screen.SetSmooth(false);
    screen.CopyScreen(*(gGame.GetRenderWindow()));
    return screen;
}
void pfx::CPostFX::SetSpriteToScreen(sf::Sprite &sprite)
{
    sprite.FlipY(true);
    sprite.Resize(static_cast<float>(gGame.GetRenderWindow()->GetWidth()),static_cast<float>(gGame.GetRenderWindow()->GetHeight()));
    sprite.SetPosition(gCamera.GetViewTopLeft());
}
