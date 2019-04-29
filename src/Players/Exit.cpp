#ifdef __unix__
#include "Exit.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Players/Exit.h"
#endif

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
