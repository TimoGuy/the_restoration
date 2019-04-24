#include "Exit.h"
#include <stdio.h>

Exit::Exit(int gx, int gy) : Object(gx, gy)
{
    //ctor
    printf("Exit built!\n");
}

Exit::~Exit()
{
    //dtor
}
