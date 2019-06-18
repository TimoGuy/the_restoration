#pragma once
#ifdef __unix__
//#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#elif defined(_WIN32) || defined(WIN32)
//#include <GL/wglew.h>
#include <SDL_opengl.h>
#endif

#include <string>

// So that users won't have to include again
#include "stb_image.h"

class Texture
{
public:
    Texture(const std::string& fileName, int desiredChannelsSTBI);                          // Image-based
    Texture( const std::string& text, const std::string& fontFileName, int fontSize);      // Font-based
    virtual ~Texture();

    void Bind(unsigned int unit);

    inline int GetWidth() const { return width; }
    inline int GetHeight() const { return height; }

private:
    GLuint m_texture;

    int width, height;

    void GenOpenGLTex(unsigned char* imgData, int comp);
};
