#if defined(__unix__) || defined(__APPLE__)
#include "TestRoom.h"
#include "TestGameObj.h"
#include "TestEnemy.h"
#include "Hazard.h"
#include "Trigger.h"
#include "MovingPlatGround.h"
#include "FuelIncreaserItem.h"
#include "InputManager.h"
#include "SerialManager.h"
#include "Textbox.h"
#include "defs.h"
#include <SDL2/SDL_ttf.h>
#include "Lib/Texture.h"
#include "Quad.h"
#include "GameLoop.h"
#include "Lib/SpriteSheetIO.h"
#include "Cutscene.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Rooms/TestRoom.h"
#include "../../include/Players/TestGameObj.h"
#include "../../include/Players/TestEnemy.h"
#include "../../include/Players/Hazard.h"
#include "../../include/Players/Trigger.h"
#include "../../include/Players/MovingPlatGround.h"
#include "../../include/Players/FuelIncreaserItem.h"
#include "../../include/InputManager.h"
#include "../../include/SerialManager.h"
#include "../../include/Players/Textbox.h"
#include "../../include/defs.h"
#include <SDL_ttf.h>
#include "../../include/Lib/Texture.h"
#include "../../include/Shape/Quad.h"
#include "../../include/GameLoop.h"
#include "../../include/Lib/SpriteSheetIO.h"
#include "../../include/Rooms/Cutscene/Cutscene.h"
#endif

#include <stdio.h>
#include <cmath>
#include <algorithm>

//class TestRoom;

#define PLAYER_WIDTH 24
#define PLAYER_HEIGHT 48
#define PLAYER_YOFF -16

// SWORD (which is really just the player's arm shooting out instantaneously) stats
// This being a low number will make the charging mini-boss harder!
// Keep in mind that the damage tick is a countdown, not an 'x steps' thing
#define SWORD_TICKS_HOLDING 40
#define SWORD_DAMAGE_TICK 20


Quad* mySword = NULL;
bool isUsingMySword = false;
bool isSwordLeft = false;
#define PLAYER_SWORD_WIDTH 128
#define PLAYER_SWORD_HEIGHT 32




TestGameObj::TestGameObj(int gx, int gy, TestRoom* rm) : Entity(gx, gy, rm)
{
    // Look for player.json to load in sprites
    std::ifstream ifs(".data/properties/player.json");
    Json::Reader reader;
    Json::Value props;
    reader.parse(ifs, props);
    sprSheet = new SpriteSheetIO(props);

    mySword = new Quad(PLAYER_SWORD_WIDTH, PLAYER_SWORD_HEIGHT);
    swordTicksLeft = 0;
    startCoords = new Quad(10, 10);

	isDoingRocketJump = false;
	rjInputActivationPhase = 0;		// The first one always
	rjInputDirection = -1;
	rjTicksInDeadzone = 0;
	rjTicksDoingRocketJump = 0;
	rjStoredHsp = 0;
	rjStoredVsp = 0;

    // Make image
    Texture* tempTex = new Texture(std::string(".data/test.png"), STBI_rgb_alpha);
    image = new Quad(PLAYER_WIDTH, PLAYER_HEIGHT, tempTex);



    // Create the tutorial
    if (!SerialManager::Instance()
            .GetGameData_Bool("saw_tutorial", GAME_VAR_DEF_saw_tutorial))
    {
        auto glambda = [&](void)
        {
            printf("Saw tutorial!\n");
            SerialManager::Instance()
                .SetGameData_Bool("saw_tutorial", true);
        };

        pf.push_back(new Textbox(x, y, std::string("td_player_tutorial"), 28, glambda, room));
    }

    printf("Player built! at %i,%i\n", gx, gy);
	y += PLAYER_YOFF;


    startX = x;
    startY = y;
    outHsp = outVsp = 0;
    lifeRechargeTicks = 0;
}

TestGameObj::~TestGameObj()
{

}

// Grav should be somewhere between 0.5 and 1.0
#define GRAV 0.5f
#define FRICTION 0.5f

// 33px (32)
#define JUMP_HEIGHT 5.5f
// 68px (64)
#define JUMP_HEIGHT_2B 8.0f
// 95px (96)
#define JUMP_HEIGHT_3B 9.5f
// 126.5px (128)
#define JUMP_HEIGHT_4B 11.0f
// 162.5px (160)
#define JUMP_HEIGHT_5B 12.5f
// 189px (192)
#define JUMP_HEIGHT_6B 13.5f

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

    if (SerialManager::Instance().GetGameData_Bool("saw_tutorial", GAME_VAR_DEF_saw_tutorial) &&
		!InputManager::Instance().b3())
    {
        inputX = InputManager::Instance().x();
		inputJump = InputManager::Instance().b2();
		isUsingMySword = IsUsingSword();

		if (!isUsingMySword)
        {
            // Thus you can't change facing direction
            // while you're holding out your sword!
            if (inputX < 0) isSwordLeft = true;
            else if (inputX > 0) isSwordLeft = false;
        }
	}



#pragma region Find Rocket Jump Input

#define RJ_ACT_I_FIRST_NEED_YOU_IN_DEADZONE 0
#define RJ_ACT_PHASE_STANDBY 1
#define RJ_ACT_PHASE_FIRST_HOLD 2
#define RJ_ACT_PHASE_BACK_TO_DEADZONE 3
#define RJ_ACT_PHASE_SECOND_HOLD 4

#define RJ_DIR_UP 0
#define RJ_DIR_DOWN 1
#define RJ_DIR_LEFT 2
#define RJ_DIR_RIGHT 3

	// Do rocket jump!!!!
	if (!isUsingMySword)
	{
#define RJ_NUM_TICKS 5
		switch (rjInputActivationPhase)
		{
		case RJ_ACT_I_FIRST_NEED_YOU_IN_DEADZONE:
		{
			// First into the deadzone threshold to begin this process!
			// (well, not technically, that's .18 for joysticks's deadzone)
			if (std::abs(InputManager::Instance().x()) < 0.5f &&
				std::abs(InputManager::Instance().y()) < 0.5f)
			{
				// Move onto next phase!
				rjTicksInDeadzone = 0;
				rjInputActivationPhase = RJ_ACT_PHASE_STANDBY;
			}
		}
			break;
		case RJ_ACT_PHASE_STANDBY:
		{
			if (std::abs(InputManager::Instance().x()) > 0.5f || 
				std::abs(InputManager::Instance().y()) > 0.5f)
			{
				// Find which direction is moving! (start w/ this eh!)
				if (std::abs(InputManager::Instance().x()) >
					std::abs(InputManager::Instance().y()))
				{
					// Okay, x-axis!!
					rjInputDirection =
						InputManager::Instance().x() < 0 ?
						RJ_DIR_LEFT : RJ_DIR_RIGHT;
				}
				else
				{
					// Fine fine, y-axis!!
					rjInputDirection =
						InputManager::Instance().y() < 0 ?
						RJ_DIR_UP : RJ_DIR_DOWN;
				}

				// It's moving now, so onto next phase!
				rjTicksInDeadzone = 0;
				rjInputActivationPhase = RJ_ACT_PHASE_FIRST_HOLD;
			}
		}
			break;
		case RJ_ACT_PHASE_FIRST_HOLD:
		{
			// Okay, this is trickiest, if you get into the same
			// direction as when you first started, you're allowed
			// into the last phase and you can do a rocket-jump!
			//
			// Within the specified amount of time too!!!
			if (rjTicksInDeadzone > RJ_NUM_TICKS)		// Half a second?? Suuure
			{
				// Reset!
				rjInputActivationPhase = RJ_ACT_I_FIRST_NEED_YOU_IN_DEADZONE;
				break;
			}
			else
			{
				// Increment ticks
				rjTicksInDeadzone++;
			}

			// Back into the no-mvt threshold eh!
			if (std::abs(InputManager::Instance().x()) < 0.5f &&
				std::abs(InputManager::Instance().y()) < 0.5f)
			{
				// Move onto next phase!
				rjTicksInDeadzone = 0;
				rjInputActivationPhase = RJ_ACT_PHASE_BACK_TO_DEADZONE;
			}
		}
			break;
		case RJ_ACT_PHASE_BACK_TO_DEADZONE:
		{
			if (rjTicksInDeadzone > RJ_NUM_TICKS)
			{
				// Reset!
				rjInputActivationPhase = RJ_ACT_I_FIRST_NEED_YOU_IN_DEADZONE;
				break;
			}
			else
			{
				// Increment ticks
				rjTicksInDeadzone++;
			}

			// Check if inputted!!
			if (std::abs(InputManager::Instance().x()) > 0.5f ||
				std::abs(InputManager::Instance().y()) > 0.5f)
			{
				// Copy of above code! (pleeeease refactor this TODO)
				int dir = -1;
				if (std::abs(InputManager::Instance().x()) >
					std::abs(InputManager::Instance().y()))
				{
					// Okay, x-axis!!
					dir =
						InputManager::Instance().x() < 0 ?
						RJ_DIR_LEFT : RJ_DIR_RIGHT;
				}
				else
				{
					// Fine fine, y-axis!!
					dir =
						InputManager::Instance().y() < 0 ?
						RJ_DIR_UP : RJ_DIR_DOWN;
				}

				// Check if direction lines up!
				if (dir == rjInputDirection)
				{
					// You pass!!!
					rjInputActivationPhase = RJ_ACT_PHASE_SECOND_HOLD;
				}
				else
				{
					// Okay, not to be mean, but I don't think you
					// meant to do a rocket jump, so we don't want
					// any false-positives....

					// Reset!
					rjInputActivationPhase = RJ_ACT_I_FIRST_NEED_YOU_IN_DEADZONE;
					break;
				}
			}
		}
			break;
		case RJ_ACT_PHASE_SECOND_HOLD:
		{
			if (numJumps > 0)		// You can deplete it to the very end, how nice of me!!
			{
				// Okay, so you passed, this is considered
				// a double-tap, and you are going to be
				// doing a rocket-jump!
				isDoingRocketJump = true;
				_freshRocketJump = true;
				rjTicksDoingRocketJump = 0;
				numJumps -= 4;
			}

			// Reset!
			rjInputActivationPhase = RJ_ACT_I_FIRST_NEED_YOU_IN_DEADZONE;
			break;
		}
		}
	}

#pragma endregion



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

	bool forceMovement =
		(isUsingMySword && swordTicksLeft == SWORD_DAMAGE_TICK) ||
		(isDoingRocketJump);
    if (forceMovement)
    {
        // If you use the sword (and it's the attacking frame), it just 'pops' out!
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












	if (!isDoingRocketJump)
	{
		// Add the input val if it was given!
		hsp += inputX;

		if (inputX == 0 &&
			hsp != 0)
		{
			if (std::abs(hsp) < FRICTION) { hsp = 0; }
			else
			{
				float chg = FRICTION * copysignf(1.0f, hsp);
				hsp -= chg;
			}
		}

		// Limit hsp
		hsp = std::min(std::max(hsp, -MAX_HSP), MAX_HSP);





		// Gravity!
		vsp += GRAV;


		// Jump!
		if (inputJump && numJumps > 0 && !wasJumpBtnAlreadyPressed)
		{
			vsp = -JUMP_HEIGHT - nerfer;
			//vsp = -JUMP_HEIGHT_2B - nerfer;
			//vsp = -JUMP_HEIGHT_3B - nerfer;
			//vsp = -JUMP_HEIGHT_4B - nerfer;		// Prob won't use 4+
			//vsp = -JUMP_HEIGHT_5B - nerfer;
			//vsp = -JUMP_HEIGHT_6B - nerfer;
			numJumps--;
		}
		wasJumpBtnAlreadyPressed = inputJump;
	}






#pragma region Process Rocket Jump Until finished

	else if (_freshRocketJump)
	{
		// Even if in the middle of another rocket
		// jump, just feel free to interrupt
		// and do another one!!
		_freshRocketJump = false;

		// Set the initial velocity (Override the input)!!
#define RJ_JUMP_VELO 80.0f
		hsp = vsp = 0;
		switch (rjInputDirection)
		{
		case RJ_DIR_UP:
			vsp = -RJ_JUMP_VELO / 2.0f;
			break;
		case RJ_DIR_DOWN:
			vsp = RJ_JUMP_VELO / 2.0f;
			break;
		case RJ_DIR_LEFT:
			hsp = -RJ_JUMP_VELO;
			break;
		case RJ_DIR_RIGHT:
			hsp = RJ_JUMP_VELO;
			break;
		}

		// Store it for phase II
		rjStoredHsp = hsp;
		rjStoredVsp = vsp;
	}
	else //if (isDoingRocketJump)
	{
#define RJ_RJ_JUMP_TOTAL_TICKS 5
#define RJ_RJ_MIDAIR_WAIT_B4_BLAST_TOTAL_TICKS 40
		// Phase II: wait in midair until windforce catches up w/ you
		if (rjTicksDoingRocketJump > RJ_RJ_MIDAIR_WAIT_B4_BLAST_TOTAL_TICKS)
		{
			// Blast into real life again!
			hsp = rjStoredHsp / 2.0f;
			vsp = rjStoredVsp / 2.0f;
			isDoingRocketJump = false;		// This undoes everything!!
			rjTicksDoingRocketJump = 0;
		}
		// Phase I: wait until actual rocket jump is done!
		else if (rjTicksDoingRocketJump > RJ_RJ_JUMP_TOTAL_TICKS)
		{
			// Stop!
			hsp = vsp = 0;
			rjTicksDoingRocketJump++;
		}
		else
		{
			rjTicksDoingRocketJump++;
		}
	}

#pragma endregion








	// Get middle of object for accuracy
    float centX = x + hsp + PLAYER_WIDTH / 2,
        centY = y + vsp + PLAYER_HEIGHT / 2;

    // Check collision potential area
	std::vector<Object*> tempCollisionsToCheck;
	SeeNeighborCollisionObjects(centX, centY, tempCollisionsToCheck);	// Inserts it into the 'tempCollisionsToCheck' variable



	// CHECK FOR COLLISION W/ THE WEAPON!!!
    if (isUsingMySword && swordTicksLeft == SWORD_DAMAGE_TICK)      // You'll hold out the sword (it'll use stamina), but the damage will actually be dealt only 1 frame!!! (So invincibility frames are not necessary)
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

                // This means still, or downwards (in comparison to the moving obj!!)
				outHsp = ((MovingPlatGround*)tempCollisions.at(i))->GetHsp();
				outVsp = ((MovingPlatGround*)tempCollisions.at(i))->GetVsp();
			}
		}
    }


    // Cycle thru all entities and react eh
    if (framesOfInvincibility <= 0)
    {
        bool hasTargeted = false;
        for (int i = (signed int)room->getEntityList()->size() - 1; i >= 0; i--)
        {
            Entity* ent;
            if ((ent = room->getEntityList()->at(i)) != this)
            {
                // It's not me, so let's interact!!!
                BoundBox b = { x + hsp, y + vsp, x, y, image->GetWidth(), image->GetHeight() };
                if (ent->IsColliding(&b))
                {
                    if (dynamic_cast<FuelIncreaserItem*>(ent) != NULL)
                    {
                        // Wow!!!! you found a fuel increase!
                        SerialManager::Instance().SetGameData_Int(
                            "player_max_jumps",
                            SerialManager::Instance().GetGameData_Int("player_max_jumps", GAME_VAR_DEF_player_max_jumps) + 1
                        );
                        ent->YouLose(this);
                    }
                    else
                    {
                        // You got hit???
                        // Do youlose() on yourself eh!
                        this->YouLose(ent);
                    }
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
		if (numJumps < SerialManager::Instance().GetGameData_Int(
                "player_max_jumps", GAME_VAR_DEF_player_max_jumps) &&
			!isUsingMySword)
			numJumps++;						// Reset number of jumps you can do

		wasJumpBtnAlreadyPressed = false;	// This allows for hold-button-jumping!
		UpdateStartCoords();
        isMidair = false;
	}
    else
        isMidair = true;

    
    
    
    // See if you're going fast enough to gain health back!!!
    #define RECHARGE_HEALTH_REQ_TICKS 50
    // printf("%i\n", lifeRechargeTicks);
    if (std::abs(hsp) > MAX_HSP - 2.0f)
    {
        // Add a tick
        lifeRechargeTicks++;


        // Enough to receive a life???
        if (lifeRechargeTicks > RECHARGE_HEALTH_REQ_TICKS)
        {
            // Reset counter
            lifeRechargeTicks = 0;

            // Award w/ 1hp (but cap off at max)
            SerialManager::Instance().SetGameData_Int(
                "player_current_health",
                std::min(
                    SerialManager::Instance().GetGameData_Int(                  // New addition of health, or...
                        "player_current_health",
                        GAME_VAR_DEF_player_current_health
                    ) + 1,
                    SerialManager::Instance().GetGameData_Int(                  // Cap it off at max hp
                        "player_max_health",
                        GAME_VAR_DEF_player_max_health
                    )
                )
            );
        }
    }
    else
    {
        lifeRechargeTicks = 0;
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



    // Set up the animation actions
    std::string action = "idle";
    if (hsp != 0) action = "run";
    if (isMidair)
    {
        action = "jump";
        if (vsp >= 0) action = "fall";
    }
    if (isUsingMySword) action = "attack";
    int flipped = isSwordLeft ? -1 : 1;
    glTranslatef(x + 24 - 12, y, 0);
    glScalef(flipped, 1, 1);
    sprSheet->Render(action, -24, 0, 48, 48);
    glScalef(flipped, 1, 1);
    glTranslatef(-x - 24 + 12, -y, 0);

	if (isUsingMySword)
	{
        float rendX = isSwordLeft ? x - PLAYER_SWORD_WIDTH : x + image->GetWidth();
        if (swordTicksLeft == SWORD_DAMAGE_TICK)
            glColor3f(1, 0, 0);
        else
            glColor4f(1, 1, 1, 0);

        mySword->Render(rendX, y + PLAYER_HEIGHT / 2 - PLAYER_SWORD_HEIGHT / 2);

        // Reset
        glColor3f(1, 1, 1);
	}

    if (InputManager::Instance().b4())
    {
        // Draw startcoords marker
        glColor4f(0.5f, 0, 0, 1);
        startCoords->Render(startX, startY);
    }


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
	if (dynamic_cast<TestEnemy*>(accordingToMe) != NULL)
	{
		// Cancel out if you aren't hazardous eh!!!
		if (!((TestEnemy*)accordingToMe)->IsHazardous())
			return;
	}

    // Enemy won :====(
    room->GetGameLoop()->AddPause(10);
    float sign = 1;
    if (x + hsp < accordingToMe->getX() ||
        hsp > 0)
    {
        sign = -1;  // If mvng right, want to shoot left as an 'ouch!'
    }

    hsp = KNOCKBACK_HSP * sign;
    vsp = KNOCKBACK_VSP;

    // Take off 1hp
    SerialManager::Instance().SetGameData_Int(
        "player_current_health",
        SerialManager::Instance().GetGameData_Int(
            "player_current_health",
            GAME_VAR_DEF_player_current_health
        ) - 23
    );
    framesOfInvincibility = HURT_FRAMES;
    

    // Check if lives are out!
    if (SerialManager::Instance().GetGameData_Int(
            "player_current_health",
            GAME_VAR_DEF_player_current_health
        ) <= 0)
    {
        // Sowee, but you ded, son
        room->GetGameLoop()->SetRoom(new Cutscene("c_dead", room->GetGameLoop()));
    }
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




bool prevB1Down = false;
bool TestGameObj::IsUsingSword()
{
    bool currentDown = InputManager::Instance().b1();

	if (swordTicksLeft == SWORD_DAMAGE_TICK + 1 &&
		currentDown)
	{
		// Hold it for a sec...
	}
	else
	{
		swordTicksLeft--;
	}

    // Can we start another cycle of sword?
    if (numJumps > 0 && swordTicksLeft <= 0)		// Sword has to be finished for that eh!
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
