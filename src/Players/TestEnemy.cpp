#ifdef __unix__
#include "TestEnemy.h"
#include "TestRoom.h"
#include "defs.h"
#elif defined(_WIN32) || defined(WIN32)
#include "..\..\include\Players\TestEnemy.h"
#include "../../include/Rooms/TestRoom.h"
#include "../../include/defs.h"
#endif

#include <algorithm>


TestEnemy::TestEnemy(int gx, int gy, TestRoom* rm)
	: Entity(gx, gy, rm)
{
	// Init
	printf("Enemy built!\n");
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
}

void TestEnemy::Render()
{
    glColor4f(1, 1, 1, life);
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
    life -= 0.5f;

    if (life <= 0)
    {
        // You ded there, son!
        delete this;
    }
}

