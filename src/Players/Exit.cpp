#include "Exit.h"
#include <stdio.h>

Exit::Exit(int gx, int gy, Room* rm) : Object(gx, gy, rm)
{
    //ctor
    printf("Exit built!\n");
}

Exit::~Exit()
{
    //dtor
}
