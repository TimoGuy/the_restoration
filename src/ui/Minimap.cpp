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









    printf("OOOOHHHH yyyeeeaaahhh!!!\n");
//    float resetX = x;   // For later
//
//    if (currentStaminas_asFloat == -1)
//    {
//        // First one
//        currentStaminas_asFloat = currentStaminas;
//    }
//    else
//    {
//        // LERP TO IT!!!!!
//        float lerp = (currentStaminas - currentStaminas_asFloat) / 2.25f;
//        currentStaminas_asFloat += lerp;
//    }
//
//    // Render setup...
//    int numStaminas = SerialManager::Instance().GetGameData_Int(
//            "player_max_jumps",
//            GAME_VAR_DEF_player_max_jumps
//        );
//    if (numStaminas <= 0) return;
//
//
//    // Render the border
//    glEnable(GL_TEXTURE_2D);
//    spriteSheetFilling->Bind(0);
//    glColor4f(0.6f, 0.5f, 1, 1);
//
//
//    if (currentStaminas > 0)
//    {
//        // Render quad!
//        glBegin(GL_QUADS);
//
//        // Render the beginning
//        glTexCoord2f(0, 0);
//        glVertex2f(x, y);
//
//        glTexCoord2f(ONE_THIRD, 0);
//        glVertex2f(x + STAMINA_CELL_WIDTH, y);
//
//        glTexCoord2f(ONE_THIRD, 1);
//        glVertex2f(x + STAMINA_CELL_WIDTH, y + STAMINA_CELL_HEIGHT);
//
//        glTexCoord2f(0, 1);
//        glVertex2f(x, y + STAMINA_CELL_HEIGHT);
//
//        // Scooch along
//        x += STAMINA_CELL_WIDTH;
//
//        // Render the filling
//        for (int i = 0; i < currentStaminas; i++)
//        {
//            // Render middle part
//            glTexCoord2f(ONE_THIRD, 0);
//            glVertex2f(x, y);
//
//            glTexCoord2f(ONE_THIRD * 2, 0);
//            glVertex2f(x + STAMINA_CELL_WIDTH, y);
//
//            glTexCoord2f(ONE_THIRD * 2, 1);
//            glVertex2f(x + STAMINA_CELL_WIDTH, y + STAMINA_CELL_HEIGHT);
//
//            glTexCoord2f(ONE_THIRD, 1);
//            glVertex2f(x, y + STAMINA_CELL_HEIGHT);
//
//
//            // Increase the x scooch
//            x += STAMINA_CELL_WIDTH;
//        }
//
//        if (currentStaminas >= numStaminas)
//        {
//            // Render the end
//            glTexCoord2f(ONE_THIRD * 2, 0);
//            glVertex2f(x, y);
//
//            glTexCoord2f(1, 0);
//            glVertex2f(x + STAMINA_CELL_WIDTH, y);
//
//            glTexCoord2f(1, 1);
//            glVertex2f(x + STAMINA_CELL_WIDTH, y + STAMINA_CELL_HEIGHT);
//
//            glTexCoord2f(ONE_THIRD * 2, 1);
//            glVertex2f(x, y + STAMINA_CELL_HEIGHT);
//        }
//        else if (currentStaminas_asFloat > currentStaminas)
//        {
//            // Add a little extra from the lerping float
//            float part = currentStaminas_asFloat - currentStaminas;     // Should be a value between 0 and 1
//            glTexCoord2f(ONE_THIRD, 0);
//            glVertex2f(x, y);
//
//            glTexCoord2f(ONE_THIRD + ONE_THIRD * part, 0);
//            glVertex2f(x + STAMINA_CELL_WIDTH * part, y);
//
//            glTexCoord2f(ONE_THIRD + ONE_THIRD * part, 1);
//            glVertex2f(x + STAMINA_CELL_WIDTH * part, y + STAMINA_CELL_HEIGHT);
//
//            glTexCoord2f(ONE_THIRD, 1);
//            glVertex2f(x, y + STAMINA_CELL_HEIGHT);
//        }
//
//        glEnd();
//    }
//
//
//
//    // Setup for border
//    x = resetX;
//    spriteSheetBorder->Bind(0);
//	glColor4f(1, 1, 1, 1);
//
//    // Render quad!
//    glBegin(GL_QUADS);
//
//    // Render Left part
//    glTexCoord2f(0, 0);
//    glVertex2f(x, y);
//
//    glTexCoord2f(ONE_THIRD, 0);
//    glVertex2f(x + STAMINA_CELL_WIDTH, y);
//
//    glTexCoord2f(ONE_THIRD, 1);
//    glVertex2f(x + STAMINA_CELL_WIDTH, y + STAMINA_CELL_HEIGHT);
//
//    glTexCoord2f(0, 1);
//    glVertex2f(x, y + STAMINA_CELL_HEIGHT);
//
//    // Scooch along
//    x += STAMINA_CELL_WIDTH;
//
//
//    for (int i = 0; i < numStaminas; i++)
//    {
//        // Render middle part
//        glTexCoord2f(ONE_THIRD, 0);
//        glVertex2f(x, y);
//
//        glTexCoord2f(ONE_THIRD * 2, 0);
//        glVertex2f(x + STAMINA_CELL_WIDTH, y);
//
//        glTexCoord2f(ONE_THIRD * 2, 1);
//        glVertex2f(x + STAMINA_CELL_WIDTH, y + STAMINA_CELL_HEIGHT);
//
//        glTexCoord2f(ONE_THIRD, 1);
//        glVertex2f(x, y + STAMINA_CELL_HEIGHT);
//
//
//        // Increase the x scooch
//        x += STAMINA_CELL_WIDTH;
//    }
//
//
//    // Render Right part
//    glTexCoord2f(ONE_THIRD * 2, 0);
//    glVertex2f(x, y);
//
//    glTexCoord2f(1, 0);
//    glVertex2f(x + STAMINA_CELL_WIDTH, y);
//
//    glTexCoord2f(1, 1);
//    glVertex2f(x + STAMINA_CELL_WIDTH, y + STAMINA_CELL_HEIGHT);
//
//    glTexCoord2f(ONE_THIRD * 2, 1);
//    glVertex2f(x, y + STAMINA_CELL_HEIGHT);
//
//    glEnd();
//
//    // Undo this mess eh!
//    glDisable(GL_TEXTURE_2D);
}
