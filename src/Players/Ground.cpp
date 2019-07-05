#ifdef __unix__
#include "Ground.h"
#include "TestRoom.h"
#include "defs.h"
#include "InputManager.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Players/Ground.h"
#include "../../include/Rooms/TestRoom.h"
#include "../../include/defs.h"
#include "../../include/InputManager.h"
#endif

#include <stdio.h>

Ground::Ground(int gx, int gy, TestRoom* rm) : Object(gx, gy, rm, true)
{
    // Init
    image = new Quad(GRID_SIZE, GRID_SIZE, new Texture(std::string(".data/textures/ground_test.png"), STBI_rgb_alpha));

    // Add to collision map
    rm->getCollisionMap()[rm->getGWidth() * gy + gx] = this;
}

Ground::~Ground()
{
    //dtor
	delete image;
}

void Ground::Render()
{
	if (InputManager::Instance().b4())
		image->Render(x, y);
}

bool Ground::IsColliding(BoundBox* box)
{
    // Use 'image' as the bounding box and test collision!
    return x < box->x + box->width &&
       x + GRID_SIZE > box->x &&
       y < box->y + box->height &&
       y + GRID_SIZE > box->y;
}
