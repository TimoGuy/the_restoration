#include "Ground.h"
#include <stdio.h>

Ground::Ground(int gx, int gy) : Object(gx, gy)
{
    //
    printf("Ground built!\n");
}

Ground::~Ground()
{
    //dtor
}
