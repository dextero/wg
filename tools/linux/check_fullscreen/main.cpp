#include <unistd.h>

#include <SFML/Graphics/RenderWindow.hpp>

int main(int argc, char* argv[]){
   
    const unsigned int windowMode = sf::Style::Close;
    const unsigned int fullscreenMode = sf::Style::Fullscreen;

    int maxw = 800; maxh = 600;
    for (size_t i = 0; i < sf::VideoMode::GetModesCount(); i++)
    {
        videoMode = sf::VideoMode::GetMode(i);
        if (videoMode.Width > maxw) {
            maxw = videoMode.Width; maxh = videoMode.Height;
        }
    }

    sf::RenderWindow * rw = new sf::RenderWindow(sf::VideoMode(maxw, maxh, 32), "Warlock's Gauntlet", fullscreenMode);
    rw->Close();
    delete rw;

    _exit(0);
}
