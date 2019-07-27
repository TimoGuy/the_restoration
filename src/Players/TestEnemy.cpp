#if defined(__unix__) || defined(__APPLE__)
#include "TestEnemy.h"
#include "TestRoom.h"
#include "defs.h"
#include "GameLoop.h"
#include "Lib/Texture.h"
#include "Quad.h"
#elif defined(_WIN32) || defined(WIN32)
#include "..\..\include\Players\TestEnemy.h"
#include "../../include/Rooms/TestRoom.h"
#include "../../include/defs.h"
#include "../../include/GameLoop.h"
#include "../../include/Lib/Texture.h"
#include "../../include/Shape/Quad.h"
#endif

#include <algorithm>


TestEnemy::TestEnemy(int gx, int gy, TestRoom* rm)
	: Entity(gx, gy, rm, true)
{
	// Init
	image = new Quad(GRID_SIZE, GRID_SIZE, new Texture(std::string(".data/textures/enemy_test.png"), STBI_rgb_alpha));
	life = 1;   // 0 is ded.
}


TestEnemy::~TestEnemy()
{
    room->RemoveEntity(this);
}



void TestEnemy::Update()
{
	// Does nothing eh
	framesOfInvincibility--;

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
    glColor4f(1, 1, 1, framesOfInvincibility > 0 ? 0.5f : 1.0f);
	image->Render(x, y);
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
    if (framesOfInvincibility > 0) return;

    life -= 0.5f;
	framesOfInvincibility = 30;
    room->GetGameLoop()->AddPause(7);

    if (life <= 0)
    {
        // You ded there, son!
        delete this;
    }
}

