#ifdef __unix__
#include "TestRoom.h"
#include "TestGameObj.h"
#include "Hazard.h"
#include "Trigger.h"
#include "MovingPlatGround.h"
#include "InputManager.h"
#include "Textbox.h"
#include "defs.h"
#include <SDL2/SDL_ttf.h>
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Rooms/TestRoom.h"
#include "../../include/Players/TestGameObj.h"
#include "../../include/Players/Hazard.h"
#include "../../include/Players/Trigger.h"
#include "../../include/Players/MovingPlatGround.h"
#include "../../include/InputManager.h"
#include "../../include/Players/Textbox.h"
#include "../../include/defs.h"
#include <SDL_ttf.h>
#endif

#include <stdio.h>
#include <cmath>
#include <algorithm>

//class TestRoom;

#define PLAYER_WIDTH 24
#define PLAYER_HEIGHT 48
#define PLAYER_YOFF -16



Quad* mySword = NULL;
bool isUsingMySword = false;
bool isSwordLeft = false;
#define PLAYER_SWORD_WIDTH 128
#define PLAYER_SWORD_HEIGHT 32




TestGameObj::TestGameObj(int gx, int gy, TestRoom* rm) : Entity(gx, gy, rm)
{
    mySword = new Quad(PLAYER_SWORD_WIDTH, PLAYER_SWORD_HEIGHT);
    startCoords = new Quad(10, 10);
    // Make image
    Texture* tempTex = new Texture(std::string(".data/test.png"), STBI_rgb_alpha);
    image = new Quad(PLAYER_WIDTH, PLAYER_HEIGHT, tempTex);



    // Create the tutorial
    if (!GameLoop::sawTutorial)
    {
        auto glambda = [&](void)
        {
            printf("Saw tutorial!\n");
            GameLoop::sawTutorial = true;
        };

        pf.push_back(new Textbox(x, y, std::string("td_player_tutorial"), 28, glambda, room));
    }

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

float reqCamOffx = 0;
#define MAX_CAM_OFFSET_X 32.0f
#define CAM_X_MVTMENT_SPEED 0.15f


void TestGameObj::Update()
{
	// Adjust according to input
	float inputX = 0;
	bool inputJump = false;

    if (GameLoop::sawTutorial &&
		!InputManager::Instance().b3())
    {
        inputX = InputManager::Instance().x();
		inputJump = InputManager::Instance().b2();
		isUsingMySword = IsUsingSword();

		if (isUsingMySword)
        {
            // Actually, you'll wanna undo the x
            // input for the duration the player's holding!
            inputX = 0;
        }
        // Thus you can't move while you're holding out your sword!
        if (inputX < 0) isSwordLeft = true;
        else if (inputX > 0) isSwordLeft = false;
	}









	// Shift SLOWLY the cam offset x!
    float multiplier = 0;
    if (isSwordLeft)
    {
        multiplier = -1;
    }
    else
    {
        multiplier = 1;
    }

    if (isUsingMySword)
    {
        // If you use the sword, it just 'pops' out!
        reqCamOffx = MAX_CAM_OFFSET_X * multiplier;
    }
    else
    {
        reqCamOffx += CAM_X_MVTMENT_SPEED * multiplier + hsp / 10.0f;
        if (reqCamOffx > MAX_CAM_OFFSET_X)
            reqCamOffx = MAX_CAM_OFFSET_X;
        else if (reqCamOffx < -MAX_CAM_OFFSET_X)
            reqCamOffx = -MAX_CAM_OFFSET_X;
    }

    room->AddCamOffCoords(reqCamOffx, 0);














	// Add the input val if it was given!
	hsp += inputX;

	if (inputX == 0 &&
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
	if (inputJump && numJumps > 0 && !wasJumpBtnAlreadyPressed)
	{
        vsp = -JUMP_HEIGHT - nerfer;
		numJumps--;
    }
	wasJumpBtnAlreadyPressed = inputJump;

    // Gravity!
    vsp += GRAV;









	// Get middle of object for accuracy
    float centX = x + hsp + PLAYER_WIDTH / 2,
        centY = y + vsp + PLAYER_HEIGHT / 2;

    // Check collision potential area
	std::vector<Object*> tempCollisionsToCheck;
	SeeNeighborCollisionObjects(centX, centY, tempCollisionsToCheck);	// Inserts it into the 'tempCollisionsToCheck' variable



	// CHECK FOR COLLISION W/ THE WEAPON!!!
    if (isUsingMySword)
    {
        for (unsigned int i = 0; i < room->getEntityList()->size(); i++)
        {
            Entity* ent;
            if ((ent = room->getEntityList()->at(i)) != this)
            {
                // It's not me, so let's interact!!!
                float swordX = isSwordLeft ? x - PLAYER_SWORD_WIDTH : x + image->GetWidth();
                float swordY = y + PLAYER_HEIGHT / 2 - PLAYER_SWORD_HEIGHT / 2;

                BoundBox b = { swordX + hsp, swordY + vsp, swordX, swordY, PLAYER_SWORD_WIDTH, PLAYER_SWORD_HEIGHT };
                if (ent->IsColliding(&b))
                {
                    ent->YouLose(this);         // Hit the enemy!!! (or npc... they won't care tho)
                }
            }
        }
    }




    // CHECK FOR COLLISION W/ NPCs/HAZARDS/DOORS/etc.
    bool die = false;


    std::vector<Object*> tempCollisions;
    if (CollideAtPos(x + hsp, y + vsp, image->GetWidth(), image->GetHeight(), &tempCollisionsToCheck, tempCollisions, false))
    {
		for (unsigned int i = 0; i < tempCollisions.size(); i++)
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
				Trigger* trig = (Trigger*)tempCollisions.at(i);

				// Already determined we're colliding, so...
				// See if it wants to trigger
				if (trig->IsDesiringToTrigger())
				{
                    // Trigger the next event eh
                    std::string evType = trig->GetNewEventID();

                    if (evType.at(0) == 'n' ||
						evType.at(0) == 'c')
                    {
                        // Go to that next room!!!!!
						Room* _room = room;
						auto rmTransLambda = [_room, trig](void)
						{
							// Switch rooms!
							Room* nextEv = (Room*)trig->GetNextEvent();		// Load the room!
							_room->GetGameLoop()->SetRoom(nextEv);

							// See if can do another screen transition (only for game rooms)
							if (dynamic_cast<TestRoom*>(nextEv) != NULL)
								((TestRoom*)nextEv)->ScreenFadeIn();
						};
						room->ScreenTransition(rmTransLambda);
                    }
                    else if (evType.at(0) == 'm')
                    {
                        // Set up a textbox!!!
                        if (pf.size() == 0)
                            pf.push_back((Textbox*)trig->GetNextEvent());
					}

					// To make sure that the trigger doesn't do anything funky!
					trig->DisableMe();
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
        bool hasTargeted = false;
        for (unsigned int i = 0; i < room->getEntityList()->size(); i++)
        {
            Entity* ent;
            if ((ent = room->getEntityList()->at(i)) != this)
            {
                // It's not me, so let's interact!!!
                BoundBox b = { x + hsp, y + vsp, x, y, image->GetWidth(), image->GetHeight() };
                if (ent->IsColliding(&b))
                {
                    // You got hit???
                    // Do youlose() on yourself eh!
                    this->YouLose(ent);
                }
            }
        }
    }




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
		if (numJumps < maxJumps &&
			!isUsingMySword)
			numJumps++;						// Reset number of jumps you can do
		wasJumpBtnAlreadyPressed = false;	// This allows for hold-button-jumping!
		UpdateStartCoords();
	}

	// Reset outside forces
	outHsp = outVsp = 0;
	framesOfInvincibility--;

    for (unsigned int i = 0; i < pf.size(); i++)
        pf.at(i)->Update();
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




	image->Render(x, y);

	if (isUsingMySword)
	{
        float rendX = isSwordLeft ? x - PLAYER_SWORD_WIDTH : x + image->GetWidth();
        mySword->Render(rendX, y + PLAYER_HEIGHT / 2 - PLAYER_SWORD_HEIGHT / 2);
	}

    glColor4f(0.5f, 0, 0, 1);
    startCoords->Render(startX, startY);



    // Render any message boxes!!!
	for (unsigned int i = 0; i < pf.size(); i++)
	{
        pf.at(i)->SetXY(x, y - 200);
        pf.at(i)->Render();
	}

	// Go thru and delete any who want to be deleted
	for (int i = pf.size() - 1; i >= 0; i--)
	{
        if (pf.at(i)->DeleteMe())
        {
            // Delete!
            pf.erase(pf.begin() + i);
        }
	}
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

int TestGameObj::GetNumJumps()
{
	return numJumps;
}





void TestGameObj::SetGridCoords(int gx, int gy)
{
    x = float(gx * GRID_SIZE);
    y = float(gy * GRID_SIZE + PLAYER_YOFF);
}

void TestGameObj::UpdateStartCoords()
{
    startX = x;
    startY = y;
}




#define SWORD_TICKS_HOLDING 25
bool prevB1Down = false;
int swordTicksLeft = 0;
bool TestGameObj::IsUsingSword()
{
    swordTicksLeft--;
    bool currentDown = InputManager::Instance().b1();

    // Can we start another cycle of sword?
    if (numJumps > 0)
    {
        // Check if JUST pressed it!
        if (!prevB1Down && currentDown)
        {
            // Go! But it uses a jump's worth of stamina eh
            swordTicksLeft = SWORD_TICKS_HOLDING;
            numJumps--;
        }
    }


    // Update prevs
    prevB1Down = currentDown;

    // This is what controls
    // if the sword is on!
    return swordTicksLeft > 0;
}
