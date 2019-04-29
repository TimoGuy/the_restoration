#ifdef __unix__
#include "Object.h"
#include "defs.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../include/Object.h"
#include "../include/defs.h"
#endif

Object::Object(int gx, int gy, Room* rm)
{
    // As you can tell,
    // gx/y means the positioning
    // within the picture grid (1 block
    // is 32x32 pixels
    x = gx * GRID_SIZE;
    y = gy * GRID_SIZE;
    room = rm;
}

Object::~Object()
{
    //dtor
}
