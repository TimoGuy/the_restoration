#ifdef __unix__
#include "Rooms/Room.h"
#include "GameLoop.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Rooms/Room.h"
#include "../../include/GameLoop.h"
#endif

Room::Room(GameLoop* gloop)
{
    _gloop = gloop;
}

Room::~Room()
{
    //dtor
}

GameLoop* Room::GetGameLoop()
{
    return _gloop;
}
