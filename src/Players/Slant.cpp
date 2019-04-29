#ifdef __unix__
#include "Slant.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Players/Slant.h"
#endif

#include <stdio.h>

Slant::Slant(int gx, int gy, Room* rm) : Object(gx, gy, rm)
{
    //ctor
    printf("Slant built!\n");
}

Slant::~Slant()
{
    //dtor
}
