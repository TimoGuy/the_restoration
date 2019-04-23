#pragma once

#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <string>

// So that users won't have to include again
#include "stb_image.h"

class Texture
{
public:
    Texture(const std::string& fileName, int desiredChannelsSTBI);
    virtual ~Texture();

    void Bind(unsigned int unit);

    inline int GetWidth() const { return width; }
    inline int GetHeight() const { return height; }

private:
    GLuint m_texture;

    int width, height;
};
