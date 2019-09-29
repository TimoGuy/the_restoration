#if defined(__unix__) || defined(__APPLE__)
#include "StaminaBar.h"
#include "SerialManager.h"
#include "Lib/Texture.h"
#include <SDL2/SDL_opengl.h>
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/ui/StaminaBar.h"
#include "../../include/SerialManager.h"
#include "../../include/Lib/Texture.h"
#include <SDL_opengl.h>
#endif

#include <string>

#define ONE_THIRD 0.3333333333333333333333333333f
#define STAMINA_CELL_WIDTH 16
#define STAMINA_CELL_HEIGHT 16

StaminaBar::StaminaBar()
{
    // Init
    currentStaminas_asFloat = -1;               // This is a codeword that it's first initialization (i.e. don't lerp it)

    // Spritesheet
    spriteSheetBorder = new Texture(std::string(".data/images/ui/stamina_bar_border.png"), STBI_rgb_alpha);
    spriteSheetFilling = new Texture(std::string(".data/images/ui/stamina_bar_fill.png"), STBI_rgb_alpha);
}

StaminaBar::~StaminaBar()
{
    // delete
    delete spriteSheetBorder;
    delete spriteSheetFilling;
}

void StaminaBar::Render(int currentStaminas, float x, float y)
{
    float resetX = x;   // For later

    if (currentStaminas_asFloat == -1)
    {
        // First one
        currentStaminas_asFloat = currentStaminas;
    }
    else
    {
        // LERP TO IT!!!!!
        float lerp = (currentStaminas - currentStaminas_asFloat) / 2.25f;
        currentStaminas_asFloat += lerp;
    }

    // Render setup...
    int numStaminas = SerialManager::Instance().GetGameData_Int(
            "player_max_jumps",
            GAME_VAR_DEF_player_max_jumps
        );
    if (numStaminas <= 0) return;


    // Render the border
    glEnable(GL_TEXTURE_2D);
    spriteSheetFilling->Bind(0);
    glColor4f(0.6f, 0.5f, 1, 1);


    if (currentStaminas > 0)
    {
        // Render quad!
        glBegin(GL_QUADS);

        // Render the beginning
        glTexCoord2f(0, 0);
        glVertex2f(x, y);

        glTexCoord2f(ONE_THIRD, 0);
        glVertex2f(x + STAMINA_CELL_WIDTH, y);

        glTexCoord2f(ONE_THIRD, 1);
        glVertex2f(x + STAMINA_CELL_WIDTH, y + STAMINA_CELL_HEIGHT);

        glTexCoord2f(0, 1);
        glVertex2f(x, y + STAMINA_CELL_HEIGHT);

        // Scooch along
        x += STAMINA_CELL_WIDTH;

        // Render the filling
        for (int i = 0; i < currentStaminas; i++)
        {
            // Render middle part
            glTexCoord2f(ONE_THIRD, 0);
            glVertex2f(x, y);

            glTexCoord2f(ONE_THIRD * 2, 0);
            glVertex2f(x + STAMINA_CELL_WIDTH, y);

            glTexCoord2f(ONE_THIRD * 2, 1);
            glVertex2f(x + STAMINA_CELL_WIDTH, y + STAMINA_CELL_HEIGHT);

            glTexCoord2f(ONE_THIRD, 1);
            glVertex2f(x, y + STAMINA_CELL_HEIGHT);


            // Increase the x scooch
            x += STAMINA_CELL_WIDTH;
        }

        if (currentStaminas >= numStaminas)
        {
            // Render the end
            glTexCoord2f(ONE_THIRD * 2, 0);
            glVertex2f(x, y);

            glTexCoord2f(1, 0);
            glVertex2f(x + STAMINA_CELL_WIDTH, y);

            glTexCoord2f(1, 1);
            glVertex2f(x + STAMINA_CELL_WIDTH, y + STAMINA_CELL_HEIGHT);

            glTexCoord2f(ONE_THIRD * 2, 1);
            glVertex2f(x, y + STAMINA_CELL_HEIGHT);
        }
        else if (currentStaminas_asFloat > currentStaminas)
        {
            // Add a little extra from the lerping float
            float part = currentStaminas_asFloat - currentStaminas;     // Should be a value between 0 and 1
            glTexCoord2f(ONE_THIRD, 0);
            glVertex2f(x, y);

            glTexCoord2f(ONE_THIRD + ONE_THIRD * part, 0);
            glVertex2f(x + STAMINA_CELL_WIDTH * part, y);

            glTexCoord2f(ONE_THIRD + ONE_THIRD * part, 1);
            glVertex2f(x + STAMINA_CELL_WIDTH * part, y + STAMINA_CELL_HEIGHT);

            glTexCoord2f(ONE_THIRD, 1);
            glVertex2f(x, y + STAMINA_CELL_HEIGHT);
        }

        glEnd();
    }



    // Setup for border
    x = resetX;
    spriteSheetBorder->Bind(0);
	glColor4f(1, 1, 1, 1);

    // Render quad!
    glBegin(GL_QUADS);

    // Render Left part
    glTexCoord2f(0, 0);
    glVertex2f(x, y);

    glTexCoord2f(ONE_THIRD, 0);
    glVertex2f(x + STAMINA_CELL_WIDTH, y);

    glTexCoord2f(ONE_THIRD, 1);
    glVertex2f(x + STAMINA_CELL_WIDTH, y + STAMINA_CELL_HEIGHT);

    glTexCoord2f(0, 1);
    glVertex2f(x, y + STAMINA_CELL_HEIGHT);

    // Scooch along
    x += STAMINA_CELL_WIDTH;


    for (int i = 0; i < numStaminas; i++)
    {
        // Render middle part
        glTexCoord2f(ONE_THIRD, 0);
        glVertex2f(x, y);

        glTexCoord2f(ONE_THIRD * 2, 0);
        glVertex2f(x + STAMINA_CELL_WIDTH, y);

        glTexCoord2f(ONE_THIRD * 2, 1);
        glVertex2f(x + STAMINA_CELL_WIDTH, y + STAMINA_CELL_HEIGHT);

        glTexCoord2f(ONE_THIRD, 1);
        glVertex2f(x, y + STAMINA_CELL_HEIGHT);


        // Increase the x scooch
        x += STAMINA_CELL_WIDTH;
    }


    // Render Right part
    glTexCoord2f(ONE_THIRD * 2, 0);
    glVertex2f(x, y);

    glTexCoord2f(1, 0);
    glVertex2f(x + STAMINA_CELL_WIDTH, y);

    glTexCoord2f(1, 1);
    glVertex2f(x + STAMINA_CELL_WIDTH, y + STAMINA_CELL_HEIGHT);

    glTexCoord2f(ONE_THIRD * 2, 1);
    glVertex2f(x, y + STAMINA_CELL_HEIGHT);

    glEnd();

    // Undo this mess eh!
    glDisable(GL_TEXTURE_2D);
}
