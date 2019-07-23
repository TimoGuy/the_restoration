#ifdef __unix__
#include "FuelIncreaserItem.h"
#include "TestRoom.h"
#include "defs.h"
#include "SerialManager.h"
#include "Lib/Texture.h"
#include "Quad.h"
#elif defined(_WIN32) || defined(WIN32)
#include "..\..\include\Players\FuelIncreaserItem.h"
#include "../../include/Rooms/TestRoom.h"
#include "../../include/defs.h"
#include "../../include/SerialManager.h"
#include "../../include/Lib/Texture.h"
#include "../../include/Shape/Quad.h"
#endif

Quad* icon = NULL;

FuelIncreaserItem::FuelIncreaserItem(int gx, int gy, TestRoom* rm)
    : Entity(gx, gy, rm)
{
    active = true;
    serial =
        room->GetLevelName() +
        std::string("_FuelIncreaserItem_") +
        std::to_string(gx) +
        std::string("_") +
        std::to_string(gy);

    // Check if serial is unique
    if (SerialManager::Instance()
            .GetJsonData()["serialized_data"]
            .isMember(serial))
    {
        // It's taken, which means it's registered as a taken object
        active = false;
    }

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
    if (!active) return;

    glColor4f(1, 1, 1, 1);
	icon->Render(x, y);
}

bool FuelIncreaserItem::IsColliding(BoundBox* box)
{
    if (!active) return false;

    // Use 'icon' as the bounding box and test collision!
	return x < box->x + box->width &&
		x + icon->GetWidth() > box->x &&
		y < box->y + box->height &&
		y + icon->GetHeight() > box->y;
}

void FuelIncreaserItem::YouLose(Entity* accordingToMe)
{
    // Okay, I go away now
    // but, register as taken!
    SerialManager::Instance().GetJsonData()["serialized_data"][serial] = true;
    SerialManager::Instance().SaveData();
    delete this;
}
