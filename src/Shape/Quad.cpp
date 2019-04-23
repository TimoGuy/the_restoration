#include "Quad.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

Quad::Quad(float width, float height)
{
    _width = width;
    _height = height;
}

Quad::~Quad()
{
    // delete _image;
}

void Quad::Render(float x, float y)
{
    // Render quad (start from lower
    // left hand corner and go counter-
    // clockwise)
    glBegin(GL_QUADS);

    glVertex2f(x, y);
    glVertex2f(x + _width, y);
    glVertex2f(x + _width, y + _height);
    glVertex2f(x, y + _height);

    glEnd();
}
