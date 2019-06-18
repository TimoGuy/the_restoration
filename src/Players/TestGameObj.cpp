#ifdef __unix__
#include "TestRoom.h"
#include "TestGameObj.h"
#include "Hazard.h"
#include "Trigger.h"
#include "MovingPlatGround.h"
#include "InputManager.h"
#include "Textbox.h"
#include "defs.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Rooms/TestRoom.h"
#include "../../include/Players/TestGameObj.h"
#include "../../include/Players/Hazard.h"
#include "../../include/Players/Trigger.h"
#include "../../include/Players/MovingPlatGround.h"
#include "../../include/InputManager.h"
#include "../../include/defs.h"
#endif

#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <cmath>
#include <algorithm>

//class TestRoom;

#define PLAYER_WIDTH 32
#define PLAYER_HEIGHT 48
#define PLAYER_YOFF -16

TestGameObj::TestGameObj(int gx, int gy, TestRoom* rm) : Entity(gx, gy, rm)
{
    // Make image
    Texture* tempTex = new Texture(std::string(".data/test.png"), STBI_rgb_alpha);
    image = new Quad(PLAYER_WIDTH, PLAYER_HEIGHT, tempTex);

//    tempTex = new Texture("Hello World!!", ".data/fonts/CATHSGBR.TTF", 28);
//    pf = new Quad(tempTex->GetWidth(), tempTex->GetHeight(), tempTex);
    pf = new Textbox(x, y, std::string("Hello\nWorld!!\nHow are you doing?\nI'm doing okay, I suppose."), 28, room);

    printf("Player built! at %i,%i\n", gx, gy);
	y += PLAYER_YOFF;


    startX = x;
    startY = y;
    outHsp = outVsp = 0;
}

TestGameObj::~TestGameObj()
{

}

// Grav should be somewhere between 0.5 and 1.0
#define GRAV 0.5f
#define FRICTION 0.5f
#define JUMP_HEIGHT 7.5f
#define MAX_HSP 35.0f

#define KNOCKBACK_HSP 10.0f
#define KNOCKBACK_VSP -4.0f
#define HURT_FRAMES 60

void TestGameObj::Update()
{
	// Adjust according to input
	hsp += InputManager::Instance().x();
	if (InputManager::Instance().x() == 0 &&
        hsp != 0)
	{
        if (abs(hsp) < FRICTION) { hsp = 0; }
        else
        {
            float chg = FRICTION * copysignf(1.0f, hsp);
            hsp -= chg;
        }
	}

    // Limit hsp
    hsp = std::min(std::max(hsp, -MAX_HSP), MAX_HSP);


    // Jump!
	if (InputManager::Instance().b2() && numJumps > 0 && !wasJumpBtnAlreadyPressed)
	{
        vsp = -JUMP_HEIGHT - nerfer;
		numJumps--;
    }
	wasJumpBtnAlreadyPressed = InputManager::Instance().b2();

    // Gravity!
    vsp += GRAV;









	// Get middle of object for accuracy
    float centX = x + hsp + PLAYER_WIDTH / 2,
        centY = y + vsp + PLAYER_HEIGHT / 2;

    // Check collision potential area
	std::vector<Object*> tempCollisionsToCheck;
	SeeNeighborCollisionObjects(centX, centY, tempCollisionsToCheck);	// Inserts it into the 'tempCollisionsToCheck' variable






    // CHECK FOR COLLISION W/ NPCs/HAZARDS/DOORS/etc.
    bool die = false;

	// TEST: if pressed 'j' then you'd automagically respawn
	if (InputManager::Instance().b1())
	{
//        printf("You dyed!!\n");
		die = true;
	}


    std::vector<Object*> tempCollisions;
    if (CollideAtPos(x + hsp, y + vsp, image->GetWidth(), image->GetHeight(), &tempCollisionsToCheck, tempCollisions, false))
    {
		for (int i = 0; i < tempCollisions.size(); i++)
		{
			// Check for hazards
			if (dynamic_cast<Hazard*>(tempCollisions.at(i)) != NULL)
			{
				// Awesome!    Now you die!
				die = true;
			}

			// Check for exits
			else if (dynamic_cast<Trigger*>(tempCollisions.at(i)) != NULL)
			{
				// Already determined we're colliding, so...
				// See if it wants to trigger
				if (((Trigger*)tempCollisions.at(i))->IsDesiringToTrigger())
				{
                    // Go to that next room!!!!!
                    room->GetGameLoop()->SetRoom(((Trigger*)tempCollisions.at(i))->GetNextEvent());
                    return;
				}
			}

			// Check for moving platforms
			else if (dynamic_cast<MovingPlatGround*>(tempCollisions.at(i)) != NULL)
			{
				// Add hsp to me!!!! (unless if hitting it from below, then you'd just hit your head!)
				//if (vsp - ((MovingPlatGround*)tempCollisions.at(i))->GetHsp() >= 0)
				{
					// This means still, or downwards (in comparison to the moving obj!!)
					outHsp = ((MovingPlatGround*)tempCollisions.at(i))->GetHsp();
					outVsp = ((MovingPlatGround*)tempCollisions.at(i))->GetVsp();
				}
			}
		}
    }


    // Cycle thru all entities and react eh
    if (framesOfInvincibility <= 0)
    {
        for (int i = 0; i < room->getEntityList()->size(); i++)
        {
            Entity* ent;
            if ((ent = room->getEntityList()->at(i)) != this)
            {
                // It's not me, so let's interact!!!
                BoundBox b = { x + hsp, y + vsp, x, y, image->GetWidth(), image->GetHeight() };
                if (ent->IsColliding(&b))
                {
                    // Hit the enemy!!!! (or you got hit???)

                    // Now, did the player win or did the enemy?
                    if (ent->getY() - GRID_SIZE >= b.prevy)
                    {
                        // Means was originally above the enemy, so the player wins!
                        hsp = 0;
                        vsp = -vsp;
                        ent->YouLose(this);
                    }
                    else
                    {
                        // Do youlose() on yourself eh!
                        this->YouLose(ent);
                    }
                }
            }
        }
    }
//    else
//        printf("You\'re invincible!!! %i\n", framesOfInvincibility);




    if (y > room->getGHeight() * GRID_SIZE)
    {
        // Fell out of the screen!!?!?!?
        die = true;
    }

    if (die)
    {
        x = startX;
        y = startY;
        hsp = vsp = 0;
    }








    // Update vsp and hsp
	hsp += outHsp;
	vsp += outVsp;
	UpdateGroundCollisionVelocity(hsp, vsp, image->GetWidth(), image->GetHeight(), &tempCollisionsToCheck);
	hsp -= outHsp;
	vsp -= outVsp;




    // CHECK STATES

	// If can jump (the difference is that
	// even after you fall off of the world,
	// you can still jump once (or how many
	// jumps you have unlocked))
	//
	// Bc now it's more of a rocket-jumping game!
	if (CollideAtPos(round(x), round(y) + 1, image->GetWidth(), image->GetHeight(), &tempCollisionsToCheck, true))
	{
		numJumps = maxJumps;	// Reset number of jumps you can do
		wasJumpBtnAlreadyPressed = false;	// This allows for hold-button-jumping!
	}

	// Reset outside forces
	outHsp = outVsp = 0;
	framesOfInvincibility--;
}

void TestGameObj::Render()
{
    if (framesOfInvincibility <= 0)
    {
        glColor4f(1, 1, 1, 1);
    }
    else
    {
        glColor4f(1, 1, 1, 0.5f);
    }
//    printf("Rendering Player!!!\n");
	image->Render(x, y);
	pf->SetCoords(x, y + 111);
	pf->Render();
}



void TestGameObj::YouLose(Entity* accordingToMe)
{
    // Enemy won :====(
    float sign = 1;
    if (x + hsp < accordingToMe->getX() ||
        hsp > 0)
    {
        sign = -1;  // If mvng right, want to shoot left as an 'ouch!'
    }

    hsp = KNOCKBACK_HSP * sign;
    vsp = KNOCKBACK_VSP;

    framesOfInvincibility = HURT_FRAMES;
}





void TestGameObj::SetGridCoords(int gx, int gy)
{
    x = gx * GRID_SIZE;
    y = gy * GRID_SIZE + PLAYER_YOFF;
}

void TestGameObj::UpdateStartCoords()
{
    startX = x;
    startY = y;
}

