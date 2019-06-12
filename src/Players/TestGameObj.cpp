#ifdef __unix__
#include "TestRoom.h"
#include "TestGameObj.h"
#include "Hazard.h"
#include "Exit.h"
#include "MovingPlatGround.h"
#include "InputManager.h"
#include "defs.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Rooms/TestRoom.h"
#include "../../include/Players/TestGameObj.h"
#include "../../include/Players/Hazard.h"
#include "../../include/Players/Exit.h"
#include "../../include/Players/MovingPlatGround.h"
#include "../../include/InputManager.h"
#include "../../include/defs.h"
#endif

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
			else if (dynamic_cast<Exit*>(tempCollisions.at(i)) != NULL)			// TODO: Maybe have the exit code handle room-switching?? Play around w/ it, and definitely do some thinking on the structure of the code systems!!!
			{
				// Already determined we're colliding, so...
				// See if it wants to trigger
				if (((Exit*)tempCollisions.at(i))->IsDesiringToTrigger())
				{
                    std::string newRmID = ((Exit*)tempCollisions.at(i))->GetNewRoomID();

					// Go to that room!!!!
					int ceGX, ceGY;
					if (((Exit*)tempCollisions.at(i))->GetCustomCoords(ceGX, ceGY))
                        room->GetGameLoop()->SetRoom(new TestRoom(newRmID, room->GetGameLoop(), ceGX, ceGY));
					else
                        room->GetGameLoop()->SetRoom(new TestRoom(newRmID, room->GetGameLoop()));

					// Game over for YOU... there will be a new player created, so don't worry!!!
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

    if (y > ((TestRoom*)room)->getGHeight() * GRID_SIZE)
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
}

void TestGameObj::Render()
{
//    printf("Rendering Player!!!\n");
	image->Render(x, y);
}




void TestGameObj::SetGridCoords(int gx, int gy)
{
    x = gx * GRID_SIZE;
    y = gy * GRID_SIZE + PLAYER_YOFF;
}

