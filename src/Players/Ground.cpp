#include "Ground.h"
#include <stdio.h>
#include "defs.h"

Ground::Ground(int gx, int gy, Room* rm) : Object(gx, gy, rm)
{
    // Init
    printf("Ground built!\n");
    image = new Quad(GRID_SIZE, GRID_SIZE, new Texture(std::string(".data/textures/ground_test.png"), STBI_rgb_alpha));

    // Add to collision map
    rm->getCollisionMap()[rm->getGWidth() * gy + gx] = this;
}

Ground::~Ground()
{
    //dtor
}

void Ground::Render()
{

	image->Render(x, y);
}

bool Ground::IsColliding(BoundBox* box)
{
    // Use 'image' as the bounding box and test collision!
    return x < box->x + box->width &&
       x + image->GetWidth() > box->x &&
       y < box->y + box->height &&
       y + image->GetHeight() > box->y;
}
