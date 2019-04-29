#include "Hazard.h"
#include "defs.h"

Hazard::Hazard(int gx, int gy, Room* rm) : Object(gx, gy, rm)
{
    // Init
    printf("Hazard built!\n");
    image = new Quad(GRID_SIZE, GRID_SIZE, new Texture(std::string(".data/textures/hazard_test.png"), STBI_rgb_alpha));

    // Add to collision map
    rm->getCollisionMap()[rm->getGWidth() * gy + gx] = this;
}

Hazard::~Hazard()
{
    //dtor
}

void Hazard::Render()
{

	image->Render(x, y);
}

bool Hazard::IsColliding(BoundBox* box)
{
    // Use 'image' as the bounding box and test collision!
    return x < box->x + box->width &&
       x + image->GetWidth() > box->x &&
       y < box->y + box->height &&
       y + image->GetHeight() > box->y;
}
