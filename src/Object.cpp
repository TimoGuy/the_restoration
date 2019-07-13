#ifdef __unix__
#include "Object.h"
#include "defs.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../include/Object.h"
#include "../include/defs.h"
#endif

Object::Object(int gx, int gy, TestRoom* rm, bool isSolid)
{
    // As you can tell,
    // gx/y means the positioning
    // within the picture grid (1 block
    // is 32x32 pixels
    SetGridCoords(gx, gy);
    room = rm;
	_isSolid = isSolid;
}

Object::~Object()
{
    //dtor
}




void Object::SetGridCoords(int gx, int gy)
{
    x = float(gx * GRID_SIZE);
    y = float(gy * GRID_SIZE);
}
