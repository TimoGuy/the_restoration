#ifdef __unix__
#include "Hazard.h"
#include "defs.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Players/Hazard.h"
#include "../../include/defs.h"
#endif

Hazard::Hazard(int gx, int gy, Room* rm) : Object(gx, gy, rm, false)
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

// The buffer's there to make the size a little smaller
#define HAZARD_BUFFER 4
bool Hazard::IsColliding(BoundBox* box)
{
    // Use 'image' as the bounding box and test collision!
    return x + HAZARD_BUFFER < box->x + box->width &&
       x + image->GetWidth() - HAZARD_BUFFER > box->x &&
       y + HAZARD_BUFFER < box->y + box->height &&
       y + image->GetHeight() - HAZARD_BUFFER > box->y;
}