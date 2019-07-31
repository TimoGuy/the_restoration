#if defined(__unix__) || defined(__APPLE__)
#include "TestMiniboss_Enemy.h"
#include "TestRoom.h"
#include "Players/TestGameObj.h"
#include "defs.h"
#include "GameLoop.h"
#include "Lib/Texture.h"
#include "Quad.h"
#elif defined(_WIN32) || defined(WIN32)
#include "..\..\include\Players\TestMiniboss_Enemy.h"
#include "../../include/Rooms/TestRoom.h"
#include "../../include/Players/TestGameObj.h"
#include "../../include/defs.h"
#include "../../include/GameLoop.h"
#include "../../include/Lib/Texture.h"
#include "../../include/Shape/Quad.h"
#endif

#include <cmath>
#include <algorithm>

#define MY_WIDTH 96
#define MY_HEIGHT 64

#define MAX_HSP 10.0f


TestMiniboss_Enemy::TestMiniboss_Enemy(int gx, int gy, TestRoom* rm)
	: Entity(gx, gy, rm, true)
{
    onGround = false;

	// Init
	image = new Quad(MY_WIDTH, MY_HEIGHT, new Texture(std::string(".data/textures/enemy_test.png"), STBI_rgb_alpha));
	life = 1;   // 0 is ded.

    // See if can find player
    targetEnt = NULL;
    FindTargetEntity();
}


TestMiniboss_Enemy::~TestMiniboss_Enemy()
{
    room->RemoveEntity(this);
}


void TestMiniboss_Enemy::Update()
{
#pragma region Find player

    // See if player is in line of sight eh.
    if (targetEnt == NULL)
        FindTargetEntity();

#pragma endregion

	// It should charge at the player when in line of sight!!!
    if (!IsTargetInBounds()) return;

	// Update mvt
    if (onGround)
    {
        hsp += x > targetEnt->getX() ? -1 : 1;

        // Check if dead
        if (life <= 0)
        {
            // You ded there, son!
            delete this;
            return;
        }
    }

    // Limit hsp
    hsp = std::min(std::max(hsp, -MAX_HSP), MAX_HSP);

    // Gravity
	vsp += 0.5f;

    // COLLISION SYSTEM
	// Get middle of object for accuracy
    float centX = x + hsp + MY_WIDTH / 2,
        centY = y + vsp + MY_HEIGHT / 2;

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
	UpdateGroundCollisionVelocity(hsp, vsp, MY_WIDTH, MY_HEIGHT, &tempCollisionsToCheck);


    // Check if on the ground
    onGround = false;
    if (CollideAtPos(round(x), round(y) + 1, MY_WIDTH, MY_HEIGHT, &tempCollisionsToCheck, true))
	{
		onGround = true;
	}
}

void TestMiniboss_Enemy::Render()
{
    glColor4f(1, 1, 1, 1);
	image->Render(x, y);
}




bool TestMiniboss_Enemy::IsColliding(BoundBox* box)
{
	// Use 'image' as the bounding box and test collision!
	return x < box->x + box->width &&
		x + image->GetWidth() > box->x &&
		y < box->y + box->height &&
		y + image->GetHeight() > box->y;
}


#define MIN_KNOCKBACK_HSP 5.0f
void TestMiniboss_Enemy::YouLose(Entity* accordingToMe)
{
    life -= 0.1f;
    room->GetGameLoop()->AddPause(7);

    // Add some knockback!
    float knockBackHsp = accordingToMe->GetHsp() * 2.0f;
    if (std::abs(knockBackHsp) > MIN_KNOCKBACK_HSP)
        hsp = knockBackHsp;
    else
        hsp = MIN_KNOCKBACK_HSP * copysignf(1.0f, knockBackHsp);

    vsp = -10;
}


void TestMiniboss_Enemy::FindTargetEntity()
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


bool TestMiniboss_Enemy::IsTargetInBounds()
{
    if (targetEnt == NULL)
    {
        printf("No target entity!\n");
        return false;
    }

    // See if x and y are within distance
    float centX = x + hsp + MY_WIDTH / 2,
        centY = y + vsp + MY_HEIGHT / 2;

    float distanceBound = GRID_SIZE * 20;        // Radius of 20 blocks, he can see you!

    float deltX = centX - targetEnt->getX(),
        deltY = centY - targetEnt->getY();

    // The distance would be smaller (in
    // bounds) than the expected distance
    return distanceBound * distanceBound > deltX * deltX + deltY * deltY;
}

