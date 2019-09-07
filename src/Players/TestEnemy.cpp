#if defined(__unix__) || defined(__APPLE__)
#include "TestEnemy.h"
#include "TestGameObj.h"
#include "TestRoom.h"
#include "defs.h"
#include "GameLoop.h"
#include "Lib/Texture.h"
#include "Quad.h"
#include "Lib/SpriteSheetIO.h"
#elif defined(_WIN32) || defined(WIN32)
#include "..\..\include\Players\TestEnemy.h"
#include "../../include/Players/TestGameObj.h"
#include "../../include/Rooms/TestRoom.h"
#include "../../include/defs.h"
#include "../../include/GameLoop.h"
#include "../../include/Lib/Texture.h"
#include "../../include/Shape/Quad.h"
#include "../../include/Lib/SpriteSheetIO.h"
#endif

#include <fstream>
#include <algorithm>

TestEnemy::TestEnemy(int gx, int gy, TestRoom* rm)
    : TestEnemy(gx, gy, rm, "properties/enemy/enemy_basic.json") { }

TestEnemy::TestEnemy(int gx, int gy, TestRoom* rm, std::string fname)
    : Entity(gx, gy, rm, true)
{
    currentAction = 0;
	isHazardous = false;
	isFacingLeft = true;
	isDying = false;
	ticksDying = 0;
    requestToNextAttackPhase = false;

    // Init
	life = 1;   // 0 is ded.

    // Load the basic Json
    std::ifstream ifs(std::string(".data/") + fname);
    Json::Reader reader;
    reader.parse(ifs, props);
    sprSheet = new SpriteSheetIO(props);

	// Load size
	myWidth = myHeight = 32;		// Default size
	if (props["sprites"]["sprite_sheet"].isMember("render_width"))
		myWidth = props["sprites"]["sprite_sheet"]["render_width"].asFloat();
	if (props["sprites"]["sprite_sheet"].isMember("render_height"))
		myHeight = props["sprites"]["sprite_sheet"]["render_height"].asFloat();

	// Adjust starting y based on height
	y -= myHeight - GRID_SIZE;

	// Load collision
	Json::Value bb = props["collision_data"];
	myBoundBox = new BoundBox();
	myBoundBox->x = bb["x_off"].asFloat();
	myBoundBox->y = bb["y_off"].asFloat();
	myBoundBox->width = bb["width"].asFloat();
	myBoundBox->height = bb["height"].asFloat();

    // See if can find player
    targetEnt = NULL;
    FindTargetEntity();
}


TestEnemy::~TestEnemy()
{
    room->RemoveEntity(this);
}

#define STATE_IDLE 1
#define STATE_TARGET 2
#define STATE_ATTACK 3

void TestEnemy::Update()
{
	// Do dying animation that's it
	if (isDying)
	{
		if (ticksDying > props["sprites"]["die_ticks"].asInt())
		{
			// Go away!!!
			delete this;
			return;
		}

		ticksDying++;
		animAction = "die";
		this->isHazardous = props["actions"]["die"]["hazardous"].asBool();
		hsp = vsp = 0;
		return;
	}


    // Find player if not yet eh
    if (targetEnt == NULL)
        FindTargetEntity();



    // Set which action it is!
    if (currentAction == 0)
    {
        // Init
        currentAction = STATE_IDLE;
    }
    else if (currentAction == STATE_IDLE &&
            !props["actions"]["target"].isNull())
    {
        // Check if targetting!
        if (IsTargetInBounds(props["actions"]["target_dist"].asFloat()))
        {
            // That means at least targeted!
			currentAction = STATE_TARGET;
        }
    }
    else if (currentAction == STATE_TARGET ||
            (currentAction == STATE_IDLE && props["actions"]["target"].isNull()))
    {
        printf("Spikking target!!\n");
        // See if want to attack
        if (IsTargetInBounds(props["actions"]["attack_dist"].asFloat()))
        {
            // Let's start the attack sequence!!!
			currentAction = STATE_ATTACK;
            attackPhase = 0;        // First attack phase!
            attackPhase_tick = 0;
        }
        else if (!IsTargetInBounds(props["actions"]["target_dist"].asFloat()))
        {
            // Target bounds,,, left them :(
			currentAction = STATE_IDLE;
        }
    }
    else if (currentAction == STATE_ATTACK)
    {
        printf("Attack Phase: %i\n", attackPhase);
        // Don't go back to idle until finished attacking!
        //if (attackPhase < 0)        // -1 is usually what is "I'm done!"
        //{
        //    currentAction = STATE_IDLE;     // Revert
        //}
    }

	// Update mvt
	vsp += 0.5f;

    // Run the AI from the json props!
    switch (currentAction)
    {
        case STATE_IDLE:
            {
                Json::Value idle = props["actions"]["idle"];
                animAction = "idle";
                ProcessAction(
                    idle["action"].asString(),
					idle["action_args"],
                    idle["hazardous"].asBool()
                );
            }
            break;

        case STATE_TARGET:
            {
                Json::Value target = props["actions"]["target"];
                animAction = "target";
                ProcessAction(
                    target["action"].asString(),
					target["action_args"],
                    target["hazardous"].asBool()
                );
            }
            break;

        case STATE_ATTACK:
            {
                Json::Value attack =
                    props["actions"]["attack_phases"][std::to_string(attackPhase)];

                // Which attack phase?
                if (attackPhase_tick > attack["phase_time_ticks"].asInt() ||
                    requestToNextAttackPhase)
                {
                    requestToNextAttackPhase = false;   // Undo flag
                    
                    // Goto next one!
                    attackPhase_tick = 0;
                    if (props["actions"]["attack_phases"]
                        .isMember(std::to_string(attackPhase + 1)))
                    {
                        attackPhase++;
                    }
                    else
                    {
                        // It's over, the attack eh
                        attackPhase = -1;
						currentAction = STATE_IDLE;		// Revert automatically
                    }
                }
                else
                {
                    // Process this attack!
                    animAction = "attack_phase_" + std::to_string(attackPhase);
                    attackPhase_tick++;
                    ProcessAction(
                        attack["action"].asString(),
						attack["action_args"],
                        attack["hazardous"].asBool()
                    );
                }

            }
            break;

        default:
            {
                printf("No state!\n");
            }
            break;
    }

    // COLLISION SYSTEM
	// Get middle of object for accuracy
    float centX = x + hsp + myWidth / 2,
        centY = y + vsp + myHeight / 2;

	// Check collision potential area
	std::vector<Object*> tempCollisionsToCheck;
	SeeNeighborCollisionObjects(centX, centY, tempCollisionsToCheck);

	// Add other enemies too!
    for (unsigned int i = 0; i < room->getEntityList()->size(); i++)
    {
        Entity* ent;
        if ((ent = room->getEntityList()->at(i)) != this)
        {
            // It's not me!!!
            tempCollisionsToCheck.push_back(ent);
        }
    }

	// Update the hsp and vsp
	UpdateGroundCollisionVelocity(hsp, vsp, myWidth, myHeight, &tempCollisionsToCheck);
}

void TestEnemy::Render()
{
	if (currentAction <= 0) return;

    glColor4f(1, 1, 1, 1);

	glTranslatef(x + (myWidth / 2), y, 0);
	if (!isFacingLeft)	glScalef(-1, 1, 1);			// Rendering'll be mirrored hehehe... whoops!
    sprSheet->Render(animAction, -(myWidth / 2), 0, myWidth, myHeight);
	if (!isFacingLeft)	glScalef(-1, 1, 1);
	glTranslatef(-x - (myWidth / 2), -y, 0);
}




bool TestEnemy::IsColliding(BoundBox* box)
{
	// Use 'image' as the bounding box and test collision!
	return x + myBoundBox->x < box->x + box->width &&
		x + myBoundBox->x + myBoundBox->width > box->x &&
		y + myBoundBox->y < box->y + box->height &&
		y + myBoundBox->y + myBoundBox->height > box->y;
}



void TestEnemy::YouLose(Entity* accordingToMe)
{
    life -= 0.5f;
    room->GetGameLoop()->AddPause(7);

    if (life <= 0)
    {
        // You ded there, son!
        //delete this;
		isDying = true;
    }
}

bool TestEnemy::IsHazardous() { return isHazardous; }

void TestEnemy::ProcessAction(std::string actionName, Json::Value actionArgs, bool isHazardous)
{
	this->isHazardous = isHazardous;

	if (targetEnt == NULL)
	{
		// Just return outta here!
		printf("No targetEnt was found. Aborting `ProcessAction()`\n");
		return;
	}

	// Assign movement behavior eh
	if (actionName == "none")
	{
		// Do nothing eh
		hsp = vsp = 0;
	}
	else if (actionName == "face_player")
	{
		isFacingLeft = targetEnt->getX() < x;
		hsp = vsp = 0;
	}
	else if (actionName == "move_towards_player")
	{
        if (actionArgs["change_direction"].asBool())
        {
            isFacingLeft = targetEnt->getX() < x;
        }

		// Assume speed is very very fast
		float argHsp = actionArgs["hsp"].asFloat() * (isFacingLeft ? -1.0f : 1.0f);
        float argVsp = actionArgs["vsp"].asFloat();
		hsp = argHsp;
        vsp = argVsp;

		// Cap off the movement if too much
		float multX = hsp > 0 ? 1.0f : -1.0f;
		float multY = vsp > 0 ? 1.0f : -1.0f;
		hsp = std::min(std::abs(hsp), std::abs(targetEnt->getX() - x)) * multX;
		vsp = std::min(std::abs(vsp), std::abs(targetEnt->getY() - y)) * multY;

        if (actionArgs.isMember("until_near"))
        {
            // Check if should move to next phase
            if (IsTargetInBounds(
                actionArgs["until_near"]["within_dist"].asFloat()
            ))
            {
                // Request!!!!!
                requestToNextAttackPhase = true;
            }
        }
	}
    else if (actionName == "move")
    {
        hsp = actionArgs["hsp"].asFloat();
        vsp = actionArgs["vsp"].asFloat();
    }
    else if (actionName == "jump_up")
    {
        hsp = 0;
        vsp = actionArgs["vsp"].asFloat();
    }
}

void TestEnemy::FindTargetEntity()
{
    // Look for the player!!!
    for (unsigned int i = 0; i < room->getEntityList()->size(); i++)
	{
        if (dynamic_cast<TestGameObj*>(room->getEntityList()->at(i)) != NULL)
        {
            // Assign!!!!
            targetEnt = room->getEntityList()->at(i);
            break;
        }
    }
}

bool TestEnemy::IsTargetInBounds(float gridRadius)
{
    if (targetEnt == NULL)
    {
        printf("No target entity!\n");
        return false;
    }

    // See if x and y are within distance
    float centX = x + hsp + myWidth / 2,
        centY = y + vsp + myHeight / 2;

    float distanceBound = GRID_SIZE * gridRadius;        // Radius of 20 blocks, he can see you!

    float deltX = centX - targetEnt->getX(),
        deltY = centY - targetEnt->getY();

    // The distance would be smaller (in
    // bounds) than the expected distance
    return distanceBound * distanceBound > deltX * deltX + deltY * deltY;
}
