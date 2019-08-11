#if defined(__unix__) || defined(__APPLE__)
#include "LifeBar.h"
#include "SerialManager.h"
#include "Lib/Texture.h"
#include <SDL2/SDL_opengl.h>
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/ui/LifeBar.h"
#include "../../include/SerialManager.h"
#include "../../include/Lib/Texture.h"
#include <SDL_opengl.h>
#endif

#define LIFE_NUM_PER_HEART 5

struct _LifeAnimationBeam
{
    float angle;
    float x, y;
};


LifeBar::LifeBar()
{
    prevLife = -1;

    // Init
    spriteSheetHearts = new Texture(std::string(".data/images/ui/heart_sprite_sheet.png"), STBI_rgb_alpha);
    texBeam = new Texture(std::string(".data/images/ui/heart_loss_beam_sheet.png"), STBI_rgb_alpha);
    // NOTE: if you animate the texbeam, make sure to give it a json animation file too!
}

LifeBar::~LifeBar()
{
    //dtor
}





struct _RenderingBounds
{
    #define SS_WIDTH 256.0
    #define SS_HEIGHT 256.0
    #define SS_CELL_WIDTH 64.0
    #define SS_CELL_HEIGHT 64.0

    float s, t, s2, t2;
    void SetBoundsFromGridPos(float gx, float gy)
    {
        s = (gx * SS_CELL_WIDTH) / SS_WIDTH;
        t = (gy * SS_CELL_HEIGHT) / SS_HEIGHT;
        s2 = ((gx * SS_CELL_WIDTH) + (SS_CELL_WIDTH - 1)) / SS_WIDTH;
        t2 = ((gy * SS_CELL_HEIGHT) + (SS_CELL_HEIGHT - 1)) / SS_HEIGHT;
    }
};

void LifeBar::Render(int currentLife, float x, float y)
{
    // Check if life has changed
    if (prevLife != currentLife ||
        prevLife == -1)
    {
        // Find out if lost life
        if (prevLife > currentLife)
        {
            // You lost
            _LifeAnimationBeam b;
            b.angle = 45;
            b.x = x;
            b.y = y;
            beams.push_back(b);
        }
    }

    // Render life
    int maxLife = SerialManager::Instance().GetGameData_Int(
            "player_max_health",
            GAME_VAR_DEF_player_max_health
        );

    // Start render the quads!
    glEnable(GL_TEXTURE_2D);
    spriteSheetHearts->Bind(0);
    glColor4f(1, 1, 1, 1);

    glBegin(GL_QUADS);

    while (maxLife > 0)
    {
        // Setup which heart to render on the sheet!
        _RenderingBounds bounds;


        if (currentLife >= LIFE_NUM_PER_HEART)
        {
            // Full heart (5 shards)
            bounds.SetBoundsFromGridPos(0, 0);
        }
        else
        {
            // You want to render one of the kind-of empty ones eh
            switch (currentLife)
            {
            case 0:
                // Empty heart!!
                bounds.SetBoundsFromGridPos(1, 1);
                break;

            case 1:
                // 1 shard
                bounds.SetBoundsFromGridPos(0, 1);
                break;

            case 2:
                // 2 shards
                bounds.SetBoundsFromGridPos(3, 0);
                break;

            case 3:
                // 3 shards
                bounds.SetBoundsFromGridPos(2, 0);
                break;

            case 4:
                // 4 shards
                bounds.SetBoundsFromGridPos(1, 0);
                break;
            
            default:
                printf("ERROR: Huh?\n");
                break;
            }
        }

        #define ONE_HEART_SIZE 24
        #define ONE_HEART_PADDING 8

        // Actually render the heart!!!
        glTexCoord2f(bounds.s, bounds.t);
        glVertex2f(x, y);

        glTexCoord2f(bounds.s2, bounds.t);
        glVertex2f(x + ONE_HEART_SIZE, y);

        glTexCoord2f(bounds.s2, bounds.t2);
        glVertex2f(x + ONE_HEART_SIZE, y + ONE_HEART_SIZE);

        glTexCoord2f(bounds.s, bounds.t2);
        glVertex2f(x, y + ONE_HEART_SIZE);

        // Keep chopping down life by 5
        maxLife -= LIFE_NUM_PER_HEART;
        currentLife -= LIFE_NUM_PER_HEART;      // Don't worry, it's a copy, so you don't actually lose life ;)

        if (currentLife < 0) currentLife = 0;

        x += ONE_HEART_SIZE + ONE_HEART_PADDING;
    }

    // End the process... bc you gotta end eventually!
    glEnd();


    // Update previous
    prevLife = currentLife;
}
