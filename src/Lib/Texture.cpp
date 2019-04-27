#include "Lib/Texture.h"
#include <stdio.h>

Texture::Texture(const std::string& fileName, int desiredChannelsSTBI)
{
    // Load image
    int comp;
    unsigned char* imgData = stbi_load(fileName.c_str(), &width, &height, &comp, desiredChannelsSTBI);

    if (imgData == NULL)
    {
        printf("Error! Texture loading failed for \"%s\"\n", fileName.c_str());
        return;
    }

    // Gen openGL texture!
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    if (comp == STBI_rgb)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
    }
    else if (comp == STBI_rgb_alpha)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
    }

    // Generate mipmaps!!!!! (req. glew.h)
//    glGenerateMipmap(GL_TEXTURE_2D);          // TODO go back and update the opengl 1.2 to glew!!! Then enable this!!!

    // Use mipmaps and enable texture clamping
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);       // TODO: get mipmapping, eh!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Free loaded image
    stbi_image_free(imgData);

    printf("Texture \"%s\" loaded\n", fileName.c_str());
}

//-----------------------------------------------------------------------------
Texture::~Texture()
{
    glDeleteTextures(1, &m_texture);
}

//-----------------------------------------------------------------------------
void Texture::Bind(unsigned int unit)
{
    if (unit < 0 || unit > 31)
    {
        printf("Error! OpenGL supports texture units up to 32 [0,32)\n");
    }

    // Bind and set
//    glActiveTexture(GL_TEXTURE0 + unit);      // Maybe this is only for opengl 3+
    glBindTexture(GL_TEXTURE_2D, m_texture);
}
