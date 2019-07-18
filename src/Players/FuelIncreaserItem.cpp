#ifdef __unix__
#include "FuelIncreaserItem.h"
#include "TestRoom.h"
#include "defs.h"
#elif defined(_WIN32) || defined(WIN32)
#include "..\..\include\Players\FuelIncreaserItem.h"
#include "../../include/Rooms/TestRoom.h"
#include "../../include/defs.h"
#endif


Quad* icon = NULL;

FuelIncreaserItem::FuelIncreaserItem(int gx, int gy, TestRoom* rm)
    : Entity(gx, gy, rm)
{
    // Init the icon if needed
    if (icon == NULL)
    {
        icon =
            new Quad(
                GRID_SIZE,
                GRID_SIZE,
                new Texture(
                    std::string(".data/textures/fuel_increaser.png"),
                    STBI_rgb_alpha
                ));
    }


}

FuelIncreaserItem::~FuelIncreaserItem()
{
    room->RemoveEntity(this);
}

void FuelIncreaserItem::Update() {}
void FuelIncreaserItem::Render()
{
    glColor4f(1, 1, 1, 1);
	icon->Render(x, y);
}

bool FuelIncreaserItem::IsColliding(BoundBox* box)
{
    // Use 'icon' as the bounding box and test collision!
	return x < box->x + box->width &&
		x + icon->GetWidth() > box->x &&
		y < box->y + box->height &&
		y + icon->GetHeight() > box->y;
}

void FuelIncreaserItem::YouLose(Entity* accordingToMe)
{
    // Okay, I go away now
    delete this;
}
