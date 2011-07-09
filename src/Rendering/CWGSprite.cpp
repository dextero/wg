#include "CWGSprite.h"

#include <SFML/Graphics/Image.hpp>
//#include <SFML/Graphics/GraphicsContext.hpp>
#include <SFML/Graphics.hpp>
#ifdef PLATFORM_MACOSX
#include <gl.h>
#else
#include <GL/gl.h>
#endif
#include <iostream>

using namespace sf;

#include "GLCheck.h"

#define GL_CLAMP_TO_EDGE    0x812F

void CWGSprite::Render(RenderTarget&) const
{
	// Get the sprite size
	float Width  = static_cast<float>(GetSubRect().GetWidth());
	float Height = static_cast<float>(GetSubRect().GetHeight());

	// Check if the image is valid
	if (GetImage())
	{
		// Set the texture
		GetImage()->Bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Calculate the texture coordinates
		bool flipX = false, flipY = false; // TODO
		FloatRect TexCoords = GetImage()->GetTexCoords(GetSubRect());
		FloatRect Rect(	flipX ? TexCoords.Right  : TexCoords.Left,
						flipY ? TexCoords.Bottom : TexCoords.Top,
						flipX ? TexCoords.Left   : TexCoords.Right,
						flipY ? TexCoords.Top    : TexCoords.Bottom);
		
		// Prepare arrays (better to make them inside sprite class and update them when it's needed istead of creating them in every render call, but that will be in future)
		Vector2f Coords[ 4 ] = {	Vector2f( Rect.Left, Rect.Top ),
									Vector2f( Rect.Left, Rect.Bottom ),
									Vector2f( Rect.Right, Rect.Bottom ),
									Vector2f( Rect.Right, Rect.Top )
									};
		
		Vector2f Vertices[ 4 ] = {	Vector2f( 0, 0 ),
									Vector2f( 0, Height ),
									Vector2f( Width, Height ),
									Vector2f( Width, 0 )
									};

		/* Draw the sprite's vertex array */
		// bind arrays
		glEnableClientState( GL_VERTEX_ARRAY );
		glVertexPointer( 2, GL_FLOAT, 0, Vertices );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 2, GL_FLOAT, 0, Coords );
		// draw arrays
		glDrawArrays( GL_QUADS, 0, 4 );
		// this is an option, maybe will be faster without disabling if rest of drawable units will use vertex array
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	}
	else
	{
		// Disable texturing
		GLCheck(glDisable(GL_TEXTURE_2D));
		
		// Prepare arrays (better to make them inside sprite class and update them when it's needed istead of creating them in every render call, but that will be in future)
		Vector2f Vertices[ 4 ] = {	Vector2f( 0, 0 ),
									Vector2f( 0, Height ),
									Vector2f( Width, Height ),
									Vector2f( Width, 0 )
									};
		
		/* Draw the sprite's vertex array */
		// bind arrays
		glEnableClientState( GL_VERTEX_ARRAY );
		glVertexPointer( 2, GL_FLOAT, 0, Vertices );
		// draw arrays
		glDrawArrays( GL_QUADS, 0, 4 );
		// this is an option, maybe will be faster without disabling if rest of drawable units will use vertex array
		glDisableClientState( GL_VERTEX_ARRAY );
	}
}
