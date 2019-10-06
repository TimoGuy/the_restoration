#if defined(__unix__) || defined(__APPLE__)
#include "TestRoom.h"
#include "TestGameObj.h"
#include "TestEnemy.h"
#include "Hazard.h"
#include "Trigger.h"
#include "MovingPlatGround.h"
#include "FuelIncreaserItem.h"
#include "Checkpoint.h"
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
#include "../../include/Players/Checkpoint.h"
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
    sprSheet = new SpriteSheetIO(props, room->GetGameLoop());

    mySword = new Quad(PLAYER_SWORD_WIDTH, PLAYER_SWORD_HEIGHT);
    swordTicksLeft = 0;
    startCoords = new Quad(10, 10);

	_isSlowMotion = false;
	_wasSlowMotionTicks = 0;

	_isRocketSpeed = false;

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


    UpdateStartCoords();
    outHsp = outVsp = 0;
    lifeRechargeTicks = 0;
}

TestGameObj::~TestGameObj()
{

}

// Grav should be somewhere between 0.5 and 1.0
#define GRAV 0.5f
#define HSP_DELTA 2.5f
#define FRICTION 0.5f
#define FRICTION_SLOW 2.5f
#define HSP_WHEN_CONSIDERED_SLOW 5.5f

// 32px
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

#define MAX_HSP 10.0f
#define MAX_HSP_FAST 35.0f
#define MAX_HSP_ATTACKING 2.5f
#define MAX_FALL_SP 14.0f

#define KNOCKBACK_HSP 10.0f
#define KNOCKBACK_VSP -4.0f
#define HURT_FRAMES 90
#define CAN_MOVE_FRAMES 60

float reqCamOffx = 0;
#define MAX_CAM_OFFSET_X 32.0f
#define CAM_X_MVTMENT_SPEED 0.15f

void TestGameObj::Update()
{
	// _SlowMotionUpdate();
	_RocketSpeedUpdate();

	// Adjust according to input
	float inputX = 0;
	bool inputJump = false;

    if (SerialManager::Instance().GetGameData_Bool("saw_tutorial", GAME_VAR_DEF_saw_tutorial) &&
		!InputManager::Instance().b3() &&
        framesOfInvincibility <= CAN_MOVE_FRAMES)
    {
        inputX = InputManager::Instance().x();
		inputJump = InputManager::Instance().b2();
		isUsingMySword = IsUsingSword();

		if (!isUsingMySword &&
            !_isRocketSpeed)
        {
            // Thus you can't change facing direction
            // while you're holding out your sword!
            if (inputX < 0) isSwordLeft = true;
            else if (inputX > 0) isSwordLeft = false;
        }
	}
    // TODO if wanted eh
    // else
    // {
    //     // Undo any delayed requests
    //     requestJump = false;
    // }




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

    if (!InputManager::Instance().b4())
    {
        bool forceMovement =
            (isUsingMySword && swordTicksLeft == SWORD_DAMAGE_TICK + 1);			// Now you will have the camera pop when you're done charging!
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
    }
    else
    {
        if (std::abs(hsp) > MAX_HSP)
        {
            // Dash refocus cam!
            int w = SCREEN_WIDTH;
            room->AddCamOffCoords(w / 2.5f * multiplier, 0);
        }
    }








	// Add the input val if it was given!
	hsp += inputX * HSP_DELTA * room->GetGameLoop()->GetGlobalTime();

	if (inputX == 0 &&
		hsp != 0)
	{
        float friction =
            std::abs(hsp) < HSP_WHEN_CONSIDERED_SLOW ? FRICTION_SLOW : FRICTION;
		if (std::abs(hsp * room->GetGameLoop()->GetGlobalTime()) <
			friction * room->GetGameLoop()->GetGlobalTime()) { hsp = 0; }
		else
		{
			float chg = friction *
				room->GetGameLoop()->GetGlobalTime() *
				copysignf(1.0f, hsp);
			hsp -= chg;
		}
	}

	// Limit hsp
	float __maxHsp__ = _isRocketSpeed ? MAX_HSP_FAST : MAX_HSP;
	__maxHsp__ =
		(isUsingMySword &&
		swordTicksLeft == SWORD_DAMAGE_TICK + 1 &&
		InputManager::Instance().b1()) ? MAX_HSP_ATTACKING : __maxHsp__;	// If charging/holding (doesn't slow down player if not holding the punch)
	hsp = std::min(std::max(hsp, -__maxHsp__), __maxHsp__);





	// Gravity!
	if (vsp < MAX_FALL_SP)
		vsp += GRAV * room->GetGameLoop()->GetGlobalTime();




    // Jump input
	if (inputJump && numJumps > 0 && !wasJumpBtnAlreadyPressed)
    {
        requestJump = true;
    }

	// Jump!
    if (requestJump && vsp >= 0)
	{
        requestJump = false;
        sprSheet->ForceAnimationFrameReset();
		vsp = -JUMP_HEIGHT - nerfer;
		//vsp = -JUMP_HEIGHT_2B - nerfer;
		//vsp = -JUMP_HEIGHT_3B - nerfer;
		//vsp = -JUMP_HEIGHT_4B - nerfer;		// Prob won't use 4+
		//vsp = -JUMP_HEIGHT_5B - nerfer;
		//vsp = -JUMP_HEIGHT_6B - nerfer;
		numJumps--;
	}
	wasJumpBtnAlreadyPressed = inputJump;








	// Get middle of object for accuracy
    float centX = x + hsp + PLAYER_WIDTH / 2,
        centY = y + vsp + PLAYER_HEIGHT / 2;

    // Check collision potential area
	std::vector<Object*> tempCollisionsToCheck;
	SeeNeighborCollisionObjects(centX, centY, tempCollisionsToCheck, true);	// Inserts it into the 'tempCollisionsToCheck' variable



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
				_Knockback(hsp > 0);
			}

            // Check for checkpoints
            else if (dynamic_cast<Checkpoint*>(tempCollisions.at(i)) != NULL)
            {
                // Set the checkpoint to this!
                UpdateStartCoords();
                room->SetCheckpoint((Checkpoint*)tempCollisions.at(i));
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
                    else if (dynamic_cast<Checkpoint*>(ent) != NULL) {}
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




    if (InputManager::Instance().b6())
    {
        // I'm dying?!?!?!?!?
        // (NOTE: player must forfeit their life... there is no automagic reset)
        die = true;
    }

    if (die)
    {
        x = startX;
        y = startY;
        hsp = vsp = 0;
    }








    // Update vsp and hsp
    float prevHsp = hsp;
	hsp += outHsp;
	vsp += outVsp;
	UpdateGroundCollisionVelocity(hsp, vsp, image->GetWidth(), image->GetHeight(), &tempCollisionsToCheck, room->GetGameLoop()->GetGlobalTime());
	hsp -= outHsp;
	vsp -= outVsp;

    if (std::abs(prevHsp) > MAX_HSP && hsp == 0)
    {
        // Hit a wall at fast speeds!
        _Knockback(prevHsp > 0);
    }


    // CHECK STATES

	// If can jump (the difference is that
	// even after you fall off of the world,
	// you can still jump once (or how many
	// jumps you have unlocked))
	if (CollideAtPos(round(x), round(y) + 1, image->GetWidth(), image->GetHeight(), &tempCollisionsToCheck, true))
	{
        numJumps =				// Reset number of jumps you can do
            SerialManager::Instance().GetGameData_Int(
                "player_max_jumps",
                GAME_VAR_DEF_player_max_jumps
            );

		wasJumpBtnAlreadyPressed = false;	// This allows for hold-button-jumping!
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
	float timePref = 1.0f;
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

	if (action == "run")
	{
		timePref = std::abs(hsp) / MAX_HSP / 1.5f;
	}
    sprSheet->Render(action, -24, 0, 48, 48, timePref);

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

    // TODO startcoords maroon box
    // if (InputManager::Instance().b4())
    // {
    //     // Draw startcoords marker
    //     glColor4f(0.5f, 0, 0, 1);
    //     startCoords->Render(startX, startY);
    // }


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

    _Knockback(x + hsp < accordingToMe->getX() || hsp > 0);
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
    if (swordTicksLeft <= 0)		// Sword has to be finished for that eh!
    {
        // Check if JUST pressed it!
        if (!prevB1Down && currentDown)
        {
            // Go!
            swordTicksLeft = SWORD_TICKS_HOLDING;
        }
    }


    // Update prevs
    prevB1Down = currentDown;

    // This is what controls
    // if the sword is on!
    return swordTicksLeft > 0;
}

#define MAX_SLOW_MOTION_TICKS 45
bool prevB5Down = false;
void TestGameObj::_SlowMotionUpdate()
{
	bool wantSwitch = !prevB5Down && InputManager::Instance().b5();
	if (wantSwitch || _wasSlowMotionTicks > MAX_SLOW_MOTION_TICKS)
	{
		_isSlowMotion = !_isSlowMotion;
		_wasSlowMotionTicks = 0;
	}

	if (_isSlowMotion)
	{
		// If holding, probs wanna do slow motion eh
		room->GetGameLoop()->SetGlobalTime(0.25f);
		// _wasSlowMotionTicks++;
	}
	else
	{
		// Reset global time haha
		room->GetGameLoop()->SetGlobalTime();
	}

	prevB5Down = InputManager::Instance().b5();
}

void TestGameObj::_RocketSpeedUpdate()
{
    // Look for the input for it eh!
    if (InputManager::Instance().b5() &&
        framesOfInvincibility <= CAN_MOVE_FRAMES)
    {
        _isRocketSpeed = true;
        int flipped = isSwordLeft ? -1 : 1;
        hsp = MAX_HSP_FAST * flipped;
    }
    // Cancel it out if too slow
    // (into the regular range again eh!)
    else if (!InputManager::Instance().b5() ||
        std::abs(hsp) <= MAX_HSP)
    {
        _isRocketSpeed = false;
    }
}

void TestGameObj::_Knockback(bool toLeft)
{
    if (framesOfInvincibility > 0) return;

    // Enemy won :====(
    room->GetGameLoop()->AddPause(10);
    float sign = 1;
    if (toLeft)
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
    _isRocketSpeed = true;      // TEMP FIX

    // Knock wind out of player!
    numJumps = 0;

    // // Check if lives are out!
    // if (SerialManager::Instance().GetGameData_Int(
    //         "player_current_health",
    //         GAME_VAR_DEF_player_current_health
    //     ) <= 0)
    // {
    //     // Sowee, but you ded, son
    //     room->GetGameLoop()->SetRoom(new Cutscene("c_dead", room->GetGameLoop()));
    // }
}
