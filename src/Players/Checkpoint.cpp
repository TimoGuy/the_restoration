#if defined(__unix__) || defined(__APPLE__)
#include "Checkpoint.h"
#include "TestRoom.h"
#include "defs.h"
#include "SerialManager.h"
#include "Lib/Texture.h"
#include "Quad.h"
#include "Lib/SpriteSheetIO.h"
#elif defined(_WIN32) || defined(WIN32)
#include "..\..\include\Players\Checkpoint.h"
#include "../../include/Rooms/TestRoom.h"
#include "../../include/defs.h"
#include "../../include/SerialManager.h"
#include "../../include/Lib/Texture.h"
#include "../../include/Shape/Quad.h"
#include "../../include/Lib/SpriteSheetIO.h"
#endif


Checkpoint::Checkpoint(int gx, int gy, TestRoom* rm)
    : Entity(gx, gy, rm)
{
    // Look for player.json to load in sprites
    std::ifstream ifs(".data/properties/checkpoint.json");
    Json::Reader reader;
    Json::Value props;
    reader.parse(ifs, props);
    sprSheet = new SpriteSheetIO(props, room->GetGameLoop());
}

Checkpoint::~Checkpoint()
{
    
}

void Checkpoint::Update() {}
void Checkpoint::Render()
{
    glColor4f(1, 1, 1, 1);

    std::string action =
        room->GetCheckpoint() == this ? "off" : "on";
    sprSheet->Render(action, 0, 0, 32, 32);
}

bool Checkpoint::IsColliding(BoundBox* box)
{
    if (!active) return false;

    // Use 'icon' as the bounding box and test collision!
	return x < box->x + box->width &&
		x + icon->GetWidth() > box->x &&
		y < box->y + box->height &&
		y + icon->GetHeight() > box->y;
}

void Checkpoint::YouLose(Entity* accordingToMe)
{
    // Okay, I go away now
    // but, register as taken!
    SerialManager::Instance().GetJsonData()["serialized_data"][serial] = true;
    SerialManager::Instance().SaveData();
    delete this;
}
