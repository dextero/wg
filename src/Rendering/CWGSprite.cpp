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

#ifdef SFML_DEBUG

    // In debug mode, perform a test on every OpenGL call
    #define GLCheck(Func) ((Func), GLCheckError(__FILE__, __LINE__))

#else

    // Else, we don't add any overhead
    #define GLCheck(Func) (Func)

#endif

#define GL_CLAMP_TO_EDGE    0x812F


////////////////////////////////////////////////////////////
/// Check the last OpenGL error
///
////////////////////////////////////////////////////////////
inline void GLCheckError(const std::string& File, unsigned int Line)
{
    // Get the last error
    GLenum ErrorCode = glGetError();

    if (ErrorCode != GL_NO_ERROR)
    {
        std::string Error = "unknown error";
        std::string Desc  = "no description";

        // Decode the error code
        switch (ErrorCode)
        {
            case GL_INVALID_ENUM :
            {
                Error = "GL_INVALID_ENUM";
                Desc  = "an unacceptable value has been specified for an enumerated argument";
                break;
            }

            case GL_INVALID_VALUE :
            {
                Error = "GL_INVALID_VALUE";
                Desc  = "a numeric argument is out of range";
                break;
            }

            case GL_INVALID_OPERATION :
            {
                Error = "GL_INVALID_OPERATION";
                Desc  = "the specified operation is not allowed in the current state";
                break;
            }

            case GL_STACK_OVERFLOW :
            {
                Error = "GL_STACK_OVERFLOW";
                Desc  = "this command would cause a stack overflow";
                break;
            }

            case GL_STACK_UNDERFLOW :
            {
                Error = "GL_STACK_UNDERFLOW";
                Desc  = "this command would cause a stack underflow";
                break;
            }

            case GL_OUT_OF_MEMORY :
            {
                Error = "GL_OUT_OF_MEMORY";
                Desc  = "there is not enough memory left to execute the command";
                break;
            }

/*            case GL_INVALID_FRAMEBUFFER_OPERATION_EXT :
            {
                Error = "GL_INVALID_FRAMEBUFFER_OPERATION_EXT";
                Desc  = "the object bound to FRAMEBUFFER_BINDING_EXT is not \"framebuffer complete\"";
                break;
            }*/
        }

        // Log the error
        std::cerr << "An internal OpenGL call failed in "
                  << File.substr(File.find_last_of("\\/") + 1) << " (" << Line << ") : "
                  << Error << ", " << Desc
                  << std::endl;
    }
}

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
