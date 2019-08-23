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

#define MY_WIDTH 96
#define MY_HEIGHT 64

TestEnemy::TestEnemy(int gx, int gy, TestRoom* rm)
    : Entity(gx, gy, rm, true)
{
    currentAction = 0;
	isHazardous = false;
	isFacingLeft = true;

    // Init
	// image = new Quad(GRID_SIZE, GRID_SIZE, new Texture(std::string(".data/textures/enemy_test.png"), STBI_rgb_alpha));
	life = 1;   // 0 is ded.

    // Load the basic Json
    std::ifstream ifs(".data/properties/enemy_basic.json");
    Json::Reader reader;
    reader.parse(ifs, props);
    sprSheet = new SpriteSheetIO(props);

    // See if can find player
    targetEnt = NULL;
    FindTargetEntity();
}

TestEnemy::TestEnemy(int gx, int gy, TestRoom* rm, Json::Value& mods)
    : TestEnemy(gx, gy, rm)
{

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
    // Find player if not yet eh
    if (targetEnt == NULL)
        FindTargetEntity();



    // Set which action it is!
    if (currentAction == 0)
    {
        // Init
        currentAction = STATE_IDLE;
    }
    else if (currentAction == STATE_IDLE)
    {
        // Check if targetting!
        if (IsTargetInBounds(20))
        {
            // That means at least targeted!
            state = STATE_TARGET;
        }
    }
    else if (currentAction == STATE_TARGET)
    {
        // See if want to attack
        if (IsTargetInBounds(6))
        {
            // Let's start the attack sequence!!!
            state = STATE_ATTACK;
            attackPhase = 0;        // First attack phase!
            attackPhase_tick = 0;
        }
        else if (!IsTargetInBounds(20))
        {
            // Target bounds,,, left them :(
            state = STATE_IDLE;
        }
    }
    else if (currentAction == STATE_ATTACK)
    {
        // Don't go back to idle until finished attacking!
        //if (attackPhase < 0)        // -1 is usually what is "I'm done!"
        //{
        //    currentAction = STATE_IDLE;     // Revert
        //}
    }



    // Run the AI from the json props!
    switch (state)
    {
        case STATE_IDLE:
            {
                Json::Value idle = props["actions"]["idle"];
                animAction = "idle";
                ProcessAction(
                    idle["action"].asString(),
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
                    target["hazardous"].asBool()
                );
            }
            break;

        case STATE_ATTACK:
            {
                Json::Value attack =
                    props["actions"]["attack_phases"][std::to_string(attackPhase)];

                // Which attack phase?
                if (attackPhase_tick > attack["phase_time_ticks"].asInt())
                {
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


	// Update mvt
	vsp += 0.5f;

    // COLLISION SYSTEM
	// Get middle of object for accuracy
    float centX = x + hsp + GRID_SIZE / 2,
        centY = y + vsp + GRID_SIZE / 2;

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
	UpdateGroundCollisionVelocity(hsp, vsp, image->GetWidth(), image->GetHeight(), &tempCollisionsToCheck);
}

void TestEnemy::Render()
{
    glColor4f(1, 1, 1, 1);
    sprSheet->Render(animAction, x, y, MY_WIDTH, MY_HEIGHT);

	// image->Render(x, y);
}




bool TestEnemy::IsColliding(BoundBox* box)
{
	// Use 'image' as the bounding box and test collision!
	return x < box->x + box->width &&
		x + image->GetWidth() > box->x &&
		y < box->y + box->height &&
		y + image->GetHeight() > box->y;
}



void TestEnemy::YouLose(Entity* accordingToMe)
{
    life -= 0.5f;
    room->GetGameLoop()->AddPause(7);

    if (life <= 0)
    {
        // You ded there, son!
        delete this;
    }
}

void TestEnemy::ProcessAction(std::string actionName, bool isHazardous)
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
	}
	else if (actionName == "face_player")
	{
		isFacingLeft = targetEnt->getX() < x;
	}
	else if (actionName == "move_towards_player")
	{
		// Assume speed is very very fast
		float speed = 20 * (isFacingLeft ? -1.0f : 1.0f);
		hsp = speed;
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
    float centX = x + hsp + MY_WIDTH / 2,
        centY = y + vsp + MY_HEIGHT / 2;

    float distanceBound = GRID_SIZE * gridRadius;        // Radius of 20 blocks, he can see you!

    float deltX = centX - targetEnt->getX(),
        deltY = centY - targetEnt->getY();

    // The distance would be smaller (in
    // bounds) than the expected distance
    return distanceBound * distanceBound > deltX * deltX + deltY * deltY;
}
