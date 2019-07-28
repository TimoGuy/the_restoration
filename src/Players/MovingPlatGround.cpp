#if defined(__unix__) || defined(__APPLE__)
#include "MovingPlatGround.h"
#include "defs.h"
#include "TestRoom.h"
#include "Lib/Texture.h"
#include "Quad.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Players/MovingPlatGround.h"
#include "../../include/defs.h"
#include "../../include/Rooms/TestRoom.h"
#include "../../include/Lib/Texture.h"
#include "../../include/Shape/Quad.h"
#endif

#include <stdio.h>
#include <cmath>
#include <algorithm>

MovingPlatGround::MovingPlatGround(int gx, int gy, TestRoom* rm) : Object(gx, gy, rm, true)
{
    // Init
    image = new Quad(GRID_SIZE, GRID_SIZE, new Texture(std::string(".data/textures/ground_test.png"), STBI_rgb_alpha));

    // Add to collision map
	int id = rm->getGWidth() * gy + gx;
    rm->getCollisionMap()[id] = this;
	collCoords.push_back(id);

	// TESTTTTTTT
	hsp = 2;
	vsp = 0;
}

MovingPlatGround::~MovingPlatGround()
{
    //dtor
	delete image;
}

void MovingPlatGround::Update()
{
	x += hsp;
	y += vsp;
	RecalcMyselfToCollisionMap();
}

void MovingPlatGround::Render()
{

	image->Render(x, y);
}

bool MovingPlatGround::IsColliding(BoundBox* box)
{
    // Use 'image' as the bounding box and test collision!
    return x < box->x + box->width &&
       x + image->GetWidth() > box->x &&
       y < box->y + box->height &&
       y + image->GetHeight() > box->y;
}



void MovingPlatGround::Extend1BlockToTheRight(int newGx, int newGy)
{
	printf("\tMPG was extended by 1!!\n");

	// Move the right point to the right more
	image->SetWidth(image->GetWidth() + GRID_SIZE);

	// Add myself to the new spots I take up too!!!
	int id = room->getGWidth() * newGy + newGx;
	room->getCollisionMap()[id] = this;
	collCoords.push_back(id);
}

void MovingPlatGround::RecalcMyselfToCollisionMap()
{
	if ((int)x == prevX && (int)y == prevY)
		return;

	// Update
	prevX = (int)x;
	prevY = (int)y;

	// Remove the previous coordinates.
	for (unsigned int i = 0; i < collCoords.size(); i++)
	{
		room->getCollisionMap()[collCoords[i]] = NULL;	// Release the spot!
	}
	collCoords.clear();

	// Re-add into the collision map and stuff
	int startGX = (int)std::floor(x) / GRID_SIZE;
	int startGY = (int)std::floor(y) / GRID_SIZE;
	int endGX = (int)std::ceil(x + image->GetWidth() - 1) / GRID_SIZE;		// The minus 1 is so that if grid-perfect, the collisions only take the minimum space
	int endGY = (int)std::ceil(y + image->GetHeight() - 1) / GRID_SIZE;

	// Make sure that gx or gy don't get outside of grid room!
	startGX = std::min(room->getGWidth() - 1, std::max(0, startGX));
	startGY = std::min(room->getGHeight() - 1, std::max(0, startGY));
	endGX = std::min(room->getGWidth() - 1, std::max(0, endGX));
	endGY = std::min(room->getGHeight() - 1, std::max(0, endGY));

	int start = room->getGWidth() * startGY + startGX;	// This does integer division. (rounds down)
	int end = room->getGWidth() * endGY + endGX;

	for (int i = start; i <= end; i++)		// Make sure to check for if 'i' gets outside of the array.
	{
		// Don't accidentally delete the already-there collisions!
		if (room->getCollisionMap()[i] != NULL)
			continue;

		room->getCollisionMap()[i] = this;	// Get this reference right into the collision map!!!!
		collCoords.push_back(i);
	}
}
