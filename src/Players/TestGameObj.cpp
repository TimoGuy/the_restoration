#include "TestGameObj.h"
#include <stdio.h>
#include <vector>
#include "InputManager.h"
#include "defs.h"

#define PLAYER_WIDTH 32
#define PLAYER_HEIGHT 48

TestGameObj::TestGameObj(int gx, int gy, Room* rm) : Object(gx, gy, rm)
{
    // Make image
    image = new Quad(PLAYER_WIDTH, PLAYER_HEIGHT);
    printf("Player built! at %i,%i\n", gx, gy);
}

TestGameObj::~TestGameObj()
{
    //dtor
}

// Grav should be somewhere between 0.5 and 1.0
#define GRAV 0.5f

void TestGameObj::Update()
{
	// Adjust according to input
	hsp += InputManager::Instance().x()*5;
	vsp += InputManager::Instance().y()*5;
	// printf("%f     %f\n", x, y);

//    vsp += GRAV;














    // Check collision
    float centX = x + PLAYER_WIDTH / 2,
        centY = y + PLAYER_HEIGHT / 2;

    int gx = centX / GRID_SIZE,     // gx and gy mean that this is where
        gy = centY / GRID_SIZE;     // the center of the obj lies in the collision grid





    // It's probably good to check the current position and the 8 around it!
    int ngx, ngy;
    std::vector<Object*> tempCollisionsToCheck;
    for (int _x = -1; _x <= 1; _x++)
    {
        for (int _y = -1; _y <= 1; _y++)
        {
            ngx = gx + _x;        // This is the offset grid, going around all 9 cells!
            ngy = gy + _y;

            // Check if the newGridCoord is out of the grid!
            if (ngx < 0 || ngx > room->getGWidth() - 1 ||
                ngy < 0 || ngy > room->getGHeight() - 1)
                continue;

            Object** tempMap = room->getCollisionMap();
            Object* tempColl = tempMap[room->getGWidth() * ngy + ngx];
            if (tempColl != NULL)
            {
                tempCollisionsToCheck.push_back(tempColl);
            }

            printf("%i,%i ", ngx, ngy);
        }
        printf("\n");
    }













    // Update x and y
    x += hsp;
    y += vsp;

    hsp = vsp = 0;















    // And then see if collided!
    for (int i = 0; i < tempCollisionsToCheck.size(); i++)
    {
        BoundBox b = { x, y, image->GetWidth(), image->GetHeight() };
        if (tempCollisionsToCheck.at(i)->IsColliding(&b))    // TODO: later I'd assume that 'image' will not be the bounding box
        {
            // Collided!
            printf("Collision!!! %i\t%i\n", ____i++, tempCollisionsToCheck.size());
            break;
        }
    }
    printf("\n\n\n\n\n\n\n\n\n");
}

void TestGameObj::Render()
{
	image->Render(x, y);
}
