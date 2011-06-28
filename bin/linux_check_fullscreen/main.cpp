#include <unistd.h>

#include <SFML/Graphics/RenderWindow.hpp>

int main(int argc, char* argv[]){
   
    const unsigned int windowMode = sf::Style::Close;
    const unsigned int fullscreenMode = sf::Style::Fullscreen;

    sf::RenderWindow * rw = new sf::RenderWindow(sf::VideoMode(800, 600, 32), "Warsztat Game", fullscreenMode);
    rw->Close();
    delete rw;

    _exit(0);
}
