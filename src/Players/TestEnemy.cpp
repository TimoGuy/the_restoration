#ifdef __unix__
#include "TestEnemy.h"
#include "defs.h"
#elif defined(_WIN32) || defined(WIN32)
#include "..\..\include\Players\TestEnemy.h"
#include "../../include/defs.h"
#endif


TestEnemy::TestEnemy(int gx, int gy, Room* rm)
	: Entity(gx, gy, rm)
{
	// Init
	printf("Enemy built!\n");
	image = new Quad(GRID_SIZE, GRID_SIZE, new Texture(std::string(".data/textures/enemy_test.png"), STBI_rgb_alpha));
}


TestEnemy::~TestEnemy()
{
}



void TestEnemy::Update()
{
	// Does nothing eh
}

void TestEnemy::Render()
{
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
