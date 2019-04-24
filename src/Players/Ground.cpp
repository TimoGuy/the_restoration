#include "Ground.h"
#include <stdio.h>
#include "defs.h"

Ground::Ground(int gx, int gy) : Object(gx, gy)
{
    //
    printf("Ground built!\n");
    image = new Quad(GRID_SIZE, GRID_SIZE);
}

Ground::~Ground()
{
    //dtor
}

void Ground::Render()
{

	image->Render(x, y);
}
