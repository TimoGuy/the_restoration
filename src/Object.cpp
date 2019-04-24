#include "Object.h"
#include "defs.h"

Object::Object(int gx, int gy)
{
    // As you can tell,
    // gx/y means the positioning
    // within the picture grid (1 block
    // is 32x32 pixels
    x = gx * GRID_SIZE;
    y = gy * GRID_SIZE;
}

Object::~Object()
{
    //dtor
}
