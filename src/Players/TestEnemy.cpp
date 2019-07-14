#ifdef __unix__
#include "TestEnemy.h"
#include "TestRoom.h"
#include "defs.h"
#include "GameLoop.h"
#elif defined(_WIN32) || defined(WIN32)
#include "..\..\include\Players\TestEnemy.h"
#include "../../include/Rooms/TestRoom.h"
#include "../../include/defs.h"
#include "../../include/GameLoop.h"
#endif

#include <algorithm>


TestEnemy::TestEnemy(int gx, int gy, TestRoom* rm)
	: Entity(gx, gy, rm)
{
	// Init
	//printf("Enemy built!\n");
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

