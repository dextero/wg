#ifndef __PART_CPARTICLE_H_
#define __PART_CPARTICLE_H_

#include <SFML/Graphics.hpp>
#include "../../Utils/MathsFunc.h"

namespace part
{
    // Struktura przechowujaca modyfikacje czasteczki
    struct SParticleMod
    {                                  // o ile ma sie zmieniac:
        float rotation;               // obrot
        float size;                   // rozmiar
        float life;                   // zycie
        sf::Vector2f force;           // wypadkowa dzialajacych sil
        // todo: kolor, textureID

        SParticleMod() : rotation(0.f), size(0.f), life(0.f) {}
    };

    // Pojedyncza cz¹steczka
    // public: <zmienna> - trick optymalizacyjny dziêki któremu kompilator mo¿e
    // zmieniæ kolejnoœæ zmiennych w strukturze tak, aby zajmowa³a jak najmniej miejsca
    // (wynika to z wyrównania)
    // 28.04.09, rAum - zmieniam CParticle na klase (moze performance nie opadnie)
    class CParticle
    {
    public: sf::Vector2f pos;           // pozycja cz¹steczki

    public: sf::Color color;            // kolor r,g,b + alpha def: 0xFFFFFFFF  
    public: float life;                 // 0-1 def: -1.f
    public: float rotation;             // 0-1 def: 0.f
    public: float size;                 // Rozmiar cz¹steczki ( najlepiej 1..64 ) def: 1.f
    public: unsigned textureID;         // nr id u¿ywanej tekstury def: 0

    public: SParticleMod mod;           // modyfikatory danej czasteczki

    CParticle() : color(255,255,255,255) ,life(-1.f), rotation(0.f), size(1.f), textureID(0) {}

    inline void Process(float dt)
    {
        life -= mod.life * dt;
        pos += mod.force * dt;
        rotation += mod.rotation * dt;
        size += mod.size * dt;

        color.a = static_cast<unsigned char>(Maths::Clamp(life * 255.f ,0.f,255.f));
    }

    };
};

#endif //__PART_CPARTICLE_H_//

