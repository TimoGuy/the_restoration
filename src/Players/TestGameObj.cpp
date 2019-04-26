#include "TestGameObj.h"
#include <stdio.h>
//#include <vector>
#include "InputManager.h"
#include "defs.h"
#include <cmath>

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
//    x += hsp;
//    y += vsp;
//
//






    // Try updating x (from hsp)
    if (!CollideAtPos(x + hsp, y, &tempCollisionsToCheck))
    {
        // It's safe!
        x += hsp;
    }
    else
    {
        // Ran into something... let's see!
        for (int tHsp = std::abs(hsp) - 1; tHsp > 0; tHsp--)
        {
            // Roll back 1 at a time and see if no collision
            float newX = x + tHsp * copysignf(1.0f, hsp);
            if (!CollideAtPos(newX, y, &tempCollisionsToCheck))
            {
                // Update!
                x = newX;
                break;
            }
        }

        // Collided so let's get this thing stopping...
        hsp = 0;
    }

    // Try updating y (from vsp)
    if (!CollideAtPos(x, y + vsp, &tempCollisionsToCheck))
    {
        // It's safe!
        y += vsp;
    }
    else
    {
        // Ran into something... let's see!
        for (int tVsp = std::abs(vsp) - 1; tVsp > 0; tVsp--)
        {
            // Roll back 1 at a time and see if no collision
            float newY = y + tVsp * copysignf(1.0f, vsp);
            if (!CollideAtPos(x, newY, &tempCollisionsToCheck))
            {
                // Update!
                y = newY;
                break;
            }
        }

        // Collided so let's get this thing stopping...
        vsp = 0;
    }



    // FOR TESTING
    hsp = vsp = 0;


}

void TestGameObj::Render()
{
	image->Render(x, y);
}








bool TestGameObj::CollideAtPos(float futX, float futY, std::vector<Object*>* collisionsToCheck)
{
    // And then see if collided!
    for (int i = 0; i < collisionsToCheck->size(); i++)
    {
        BoundBox b = { futX, futY, image->GetWidth(), image->GetHeight() };
        if (collisionsToCheck->at(i)->IsColliding(&b))    // TODO: later I'd assume that 'image' will not be the bounding box
        {
            // Collided!
//            printf("Collision!!! %i\t%i\n", ____i++, collisionsToCheck.size());
            return true;
//            break;
        }
    }
//    printf("\n\n\n\n\n\n\n\n\n");
    return false;
}
