#if defined(__unix__) || defined(__APPLE__)
#include "Minimap.h"
#include "Lib/Texture.h"
#include <SDL2/SDL_opengl.h>
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/ui/Minimap.h"
#include "../../include/Lib/Texture.h"
#include <SDL_opengl.h>
#endif

#define MM_SIZE 250
#define MM_SCALE 2.0f

Minimap::Minimap(std::string picFname)
{
    mapTexture = new Texture(picFname, STBI_rgb_alpha);
}

Minimap::~Minimap()
{
    delete mapTexture;
}

void Minimap::Render(float rendX, float rendY, int centGX, int centGY)
{
    // Calculate bounds to show
    float stretchX = (float)centGX / mapTexture->GetWidth();
    float stretchY = (float)centGY / mapTexture->GetHeight();
    float stretchW = (float)MM_SIZE / mapTexture->GetWidth() / MM_SCALE;
    float stretchH = (float)MM_SIZE / mapTexture->GetHeight() / MM_SCALE;

    // Shift
    stretchX -= stretchW / 2.0f;
    stretchY -= stretchH / 2.0f;

    // Setup render
    glEnable(GL_TEXTURE_2D);
    mapTexture->Bind(0);
    glColor4f(1, 1, 1, 0.5f);

    // Render
    glBegin(GL_QUADS);

    glTexCoord2f(stretchX, stretchY);
    glVertex2f(rendX, rendY);

    glTexCoord2f(stretchX + stretchW, stretchY);
    glVertex2f(rendX + MM_SIZE, rendY);

    glTexCoord2f(stretchX + stretchW, stretchY + stretchH);
    glVertex2f(rendX + MM_SIZE, rendY + MM_SIZE);

    glTexCoord2f(stretchX, stretchY + stretchH);
    glVertex2f(rendX, rendY + MM_SIZE);

    glEnd();

    // Undo this mess eh!
    glDisable(GL_TEXTURE_2D);

    
    // Render crosshairs
    glColor4f(1, 0.5f, 0, 1);

    glBegin(GL_LINES);

    glVertex2f(rendX, rendY + MM_SIZE / 2.0f);
    glVertex2f(rendX + MM_SIZE, rendY + MM_SIZE / 2.0f);

    glVertex2f(rendX + MM_SIZE / 2.0f, rendY);
    glVertex2f(rendX + MM_SIZE / 2.0f, rendY + MM_SIZE);

    glEnd();
}
