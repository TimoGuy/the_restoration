#include "Slant.h"
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
