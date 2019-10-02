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

    myBoundBox = new BoundBox();
    myBoundBox->x = 0;
    myBoundBox->y = 0;
    myBoundBox->width = 32;
    myBoundBox->height = 32;
}

Checkpoint::~Checkpoint()
{

}

void Checkpoint::Update() {}
void Checkpoint::Render()
{
    glColor4f(1, 1, 1, 1);

    std::string action =
        room->GetCheckpoint() == this ? "on" : "off";
    sprSheet->Render(action, x, y, 32, 32);
}

bool Checkpoint::IsColliding(BoundBox* box)
{
    return x + myBoundBox->x < box->x + box->width &&
		x + myBoundBox->x + myBoundBox->width > box->x &&
		y + myBoundBox->y < box->y + box->height &&
		y + myBoundBox->y + myBoundBox->height > box->y;
}

void Checkpoint::YouLose(Entity* accordingToMe) {}
