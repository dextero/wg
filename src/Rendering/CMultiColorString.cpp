#include "CMultiColorString.h"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Unicode.hpp>
#include <windows.h>
#include <GL/gl.h>
#include <locale>

using namespace sf;

// wartosc uzywana do zapisu nastepnego koloru
#define MAGIC_CHAR L'\u1680'

// jakies makro sfmlowe, nie moge go znalezc
#define GLCheck(a) a

const std::wstring CMultiColorString::NextColor(sf::Color col)
{
    // tak! std::wstring moze miec nulle w srodku :3
    wchar_t buffer[4];
    buffer[0] = MAGIC_CHAR;
    buffer[1] = ((const wchar_t*)&col)[0];
    buffer[2] = ((const wchar_t*)&col)[1];
    buffer[3] = L'\0';

    return std::wstring(buffer, 3);
}

void CMultiColorString::Render(sf::RenderTarget&) const
{
    const Unicode::Text& myText = String::GetText();
    const Font& myFont = String::GetFont();
    float mySize = String::GetSize();
    unsigned long myStyle = String::GetStyle();

    // First get the internal UTF-32 string of the text
    const Unicode::UTF32String& Text = myText;

    // No text, no rendering :)
    if (Text.empty())
        return;

    // Set the scaling factor to get the actual size
    float CharSize =  static_cast<float>(myFont.GetCharacterSize());
    float Factor   = mySize / CharSize;
    GLCheck(glScalef(Factor, Factor, 1.f));

    // Bind the font texture
    myFont.GetImage().Bind();

    // Initialize the rendering coordinates
    float X = 0.f;
    float Y = CharSize;

    // Holds the lines to draw later, for underlined style
    std::vector<float> UnderlineCoords;
    UnderlineCoords.reserve(16);

    // Compute the shearing to apply if we're using the italic style
    float ItalicCoeff = (myStyle & Italic) ? 0.208f : 0.f; // 12 degrees

    // Draw one quad for each character
    glBegin(GL_QUADS);
    for (std::size_t i = 0; i < Text.size(); ++i)
    {
        // jesli mamy magic_char i sa za nim jeszcze przynajmniej 2 znaki, to zmien kolor
        if (Text[i] == MAGIC_CHAR && Text.size() > i + 2)
        {
            unsigned int value = Text[i + 1] | (Text[i + 2] << 16);
            sf::Color col = *(sf::Color*)&value;
            GLCheck(glColor4ub(col.r, col.g, col.b, col.a));
            i += 2; // MAGIC_CHAR i 2 znaki na kolor, ale jest jeszcze ++i
            continue;
        }

        // Get the current character and its corresponding glyph
        Uint32           CurChar  = Text[i];
        const Glyph&     CurGlyph = myFont.GetGlyph(CurChar);
        int              Advance  = CurGlyph.Advance;
        const IntRect&   Rect     = CurGlyph.Rectangle;
        const FloatRect& Coord    = CurGlyph.TexCoords;

        // If we're using the underlined style and there's a new line,
        // we keep track of the previous line to draw it later
        if ((CurChar == L'\n') && (myStyle & Underlined))
        {
            UnderlineCoords.push_back(X);
            UnderlineCoords.push_back(Y + 2);
        }

        // Handle special characters
        switch (CurChar)
        {
            case L' ' :  X += Advance;         continue;
            case L'\n' : Y += CharSize; X = 0; continue;
            case L'\t' : X += Advance  * 4;    continue;
            case L'\v' : Y += CharSize * 4;    continue;
        }

        // Draw a textured quad for the current character
        glTexCoord2f(Coord.Left,  Coord.Top);    glVertex2f(X + Rect.Left  - ItalicCoeff * Rect.Top,    Y + Rect.Top);
        glTexCoord2f(Coord.Left,  Coord.Bottom); glVertex2f(X + Rect.Left  - ItalicCoeff * Rect.Bottom, Y + Rect.Bottom);
        glTexCoord2f(Coord.Right, Coord.Bottom); glVertex2f(X + Rect.Right - ItalicCoeff * Rect.Bottom, Y + Rect.Bottom);
        glTexCoord2f(Coord.Right, Coord.Top);    glVertex2f(X + Rect.Right - ItalicCoeff * Rect.Top,    Y + Rect.Top);

        // If we're using the bold style, we must render the character 4 more times,
        // slightly offseted, to simulate a higher weight
        if (myStyle & Bold)
        {
            static const float OffsetsX[] = {-0.5f, 0.5f, 0.f, 0.f};
            static const float OffsetsY[] = {0.f, 0.f, -0.5f, 0.5f};

            for (int j = 0; j < 4; ++j)
            {
                glTexCoord2f(Coord.Left,  Coord.Top);    glVertex2f(X + OffsetsX[j] + Rect.Left  - ItalicCoeff * Rect.Top,    Y + OffsetsY[j] + Rect.Top);
                glTexCoord2f(Coord.Left,  Coord.Bottom); glVertex2f(X + OffsetsX[j] + Rect.Left  - ItalicCoeff * Rect.Bottom, Y + OffsetsY[j] + Rect.Bottom);
                glTexCoord2f(Coord.Right, Coord.Bottom); glVertex2f(X + OffsetsX[j] + Rect.Right - ItalicCoeff * Rect.Bottom, Y + OffsetsY[j] + Rect.Bottom);
                glTexCoord2f(Coord.Right, Coord.Top);    glVertex2f(X + OffsetsX[j] + Rect.Right - ItalicCoeff * Rect.Top,    Y + OffsetsY[j] + Rect.Top);
            }
        }

        // Advance to the next character
        X += Advance;
    }
    glEnd();

    // Draw the underlines if needed
    if (myStyle & Underlined)
    {
        // Compute the line thickness
        float Thickness = (myStyle & Bold) ? 3.f : 2.f;

        // Add the last line (which was not finished with a \n)
        UnderlineCoords.push_back(X);
        UnderlineCoords.push_back(Y + 2);

        // Draw the underlines as quads
        GLCheck(glDisable(GL_TEXTURE_2D));
        glBegin(GL_QUADS);
        for (std::size_t i = 0; i < UnderlineCoords.size(); i += 2)
        {
            glVertex2f(0,                  UnderlineCoords[i + 1]);
            glVertex2f(0,                  UnderlineCoords[i + 1] + Thickness);
            glVertex2f(UnderlineCoords[i], UnderlineCoords[i + 1] + Thickness);
            glVertex2f(UnderlineCoords[i], UnderlineCoords[i + 1]);
        }
        glEnd();
    }
}
