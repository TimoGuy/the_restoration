#ifdef __unix__
#include "Quad.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Shape/Quad.h"			// WOWEE windows is wEIRD!!!
#include <SDL.h>
#include <SDL_opengl.h>
#endif


Quad::Quad(float width, float height)
{
    _width = width;
    _height = height;
}

Quad::Quad(float width, float height, Texture* tex) : Quad(width, height)
{
    _image = tex;
}

Quad::~Quad()
{
     delete _image;
}

void Quad::Render(float x, float y)
{
    if (_image != NULL)
    {
        // Enable textures
        glEnable(GL_TEXTURE_2D);
        _image->Bind(0);
    }

    // Render quad (start from lower
    // left hand corner and go counter-
    // clockwise)
    glBegin(GL_QUADS);

    glTexCoord2f(0, 0);
    glVertex2f(x, y);

    glTexCoord2f(1, 0);
    glVertex2f(x + _width, y);

    glTexCoord2f(1, 1);
    glVertex2f(x + _width, y + _height);

    glTexCoord2f(0, 1);
    glVertex2f(x, y + _height);

    glEnd();



    if (_image != NULL)
    {
        // Undo textures
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, NULL);
    }
}
