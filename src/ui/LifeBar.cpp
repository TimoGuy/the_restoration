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

    SerialManager::Instance().SetGameData_Int(
        "player_current_health",
        20
    );          // DEBUG

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
    #define ONE_HEART_SIZE 24
    #define ONE_HEART_PADDING 8
    
    // Check if life has changed
    if (prevLife != currentLife ||
        prevLife == -1)
    {
        // Find out if lost life
        if (prevLife > currentLife)
        {
            // Actually, only do 1 at a time!!!!
            currentLife = prevLife - 1;

            // You lost
            _LifeAnimationBeam b;
            int whichHeart = currentLife / LIFE_NUM_PER_HEART;
            int whichAngle = currentLife % LIFE_NUM_PER_HEART + 1;
            
            b.angle = 360 / LIFE_NUM_PER_HEART * (360 - whichAngle) - 54;
            b.x = x + (ONE_HEART_SIZE + ONE_HEART_PADDING) * (float)whichHeart;
            b.y = y;

            // Do some advanced shifting
            switch (whichAngle)
            {
                case 5:
                    b.x += (45 / SS_CELL_WIDTH) * ONE_HEART_SIZE;
                    b.y += (20 / SS_CELL_HEIGHT) * ONE_HEART_SIZE;
                    break;
                    
                case 4:
                    b.x += (41 / SS_CELL_WIDTH) * ONE_HEART_SIZE;
                    b.y += (34 / SS_CELL_HEIGHT) * ONE_HEART_SIZE;
                    break;
                    
                case 3:
                    b.x += (32 / SS_CELL_WIDTH) * ONE_HEART_SIZE;
                    b.y += (42 / SS_CELL_HEIGHT) * ONE_HEART_SIZE;
                    break;
                    
                case 2:
                    b.x += (23 / SS_CELL_WIDTH) * ONE_HEART_SIZE;
                    b.y += (34 / SS_CELL_HEIGHT) * ONE_HEART_SIZE;
                    break;
                    
                case 1:
                    b.x += (16 / SS_CELL_WIDTH) * ONE_HEART_SIZE;
                    b.y += (20 / SS_CELL_HEIGHT) * ONE_HEART_SIZE;
                    break;
            }

            beams.push_back(b);

            printf("Following!!!    %i\n", whichAngle);
        }
    }

    // For the prev variable l8r eh
    int origCurrentLife = currentLife;

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

    // End the process.
    glEnd();



    // Render the beams
    texBeam->Bind(0);

    #define BEAM_XOFF -16
    #define BEAM_YOFF -32
    for (unsigned int i = 0; i < beams.size(); i++)
    {
        glTranslatef(beams.at(i).x, beams.at(i).y, 0);
        glRotatef(beams.at(i).angle, 0, 0, 1);
        
        glBegin(GL_QUADS);

        glTexCoord2f(0, 0);
        glVertex2f(BEAM_XOFF, BEAM_YOFF);

        glTexCoord2f(1, 0);
        glVertex2f(BEAM_XOFF + texBeam->GetWidth(), BEAM_YOFF);

        glTexCoord2f(1, 1);
        glVertex2f(BEAM_XOFF + texBeam->GetWidth(), BEAM_YOFF + texBeam->GetHeight());

        glTexCoord2f(0, 1);
        glVertex2f(BEAM_XOFF, BEAM_YOFF + texBeam->GetHeight());

        // End the process... bc you gotta end eventually!
        glEnd();

        glRotatef(-beams.at(i).angle, 0, 0, 1);
        glTranslatef(-beams.at(i).x, -beams.at(i).y, 0);
    }

    beams.clear();


    // Update previous
    prevLife = origCurrentLife;
}
