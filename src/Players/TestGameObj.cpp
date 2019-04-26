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
#define FRICTION 0.5f
#define JUMP_HEIGHT 7.5f

void TestGameObj::Update()
{
	// Adjust according to input
	hsp += InputManager::Instance().x();
	if (InputManager::Instance().x() == 0 &&
        hsp != 0)
	{
        if (abs(hsp) < FRICTION) { hsp = 0; }
        else
        {
            float chg = FRICTION * copysignf(1.0f, hsp);
//            printf("%f\n", copysignf(1.0f, hsp));
            hsp -= chg;
        }
	}

	if (InputManager::Instance().b2() && isOnGround)
	{
        printf("Jumping!\n");
        vsp -= JUMP_HEIGHT - nerfer;
    }

//	vsp += InputManager::Instance().y()*5;
	// printf("%f     %f\n", x, y);

    vsp += GRAV;        // Gravity!














    // Check collision
    float centX = x + hsp + PLAYER_WIDTH / 2,
        centY = y + vsp + PLAYER_HEIGHT / 2;

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

//            printf("%i,%i ", ngx, ngy);
        }
//        printf("\n");
    }













    // Update x and y
//    x += hsp;
//    y += vsp;
//
//






    // Try updating x (from hsp)
    int tHsp = ceil(abs(hsp));
    if (!CollideAtPos(x + tHsp * copysignf(1.0f, hsp), y, &tempCollisionsToCheck))
    {
        // It's safe!
        x += tHsp * copysignf(1.0f, hsp);
    }
    else if (tHsp > 0)
    {
        // Ran into something... let's see!
        for (tHsp -= 1; tHsp > 0; tHsp--)
        {
            // Roll back 1 at a time and see if no collision
            int newX = round(x) + tHsp * copysignf(1.0f, hsp);
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
    int tVsp = ceil(abs(vsp));
    if (!CollideAtPos(x, y + tVsp * copysignf(1.0f, vsp), &tempCollisionsToCheck))
    {
        // It's safe!
        y += tVsp * copysignf(1.0f, vsp);
    }
    else if (tVsp > 0)
    {
//        if (vsp < 0)
//        {
//            int donothing = 0;
//        }
        // Ran into something... let's see!
        for (tVsp -= 1; tVsp > 0; tVsp--)
        {
            // Roll back 1 at a time and see if no collision
            int newY = round(y) + tVsp * copysignf(1.0f, vsp);
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
//    hsp = vsp = 0;







    // Check states
    isOnGround = CollideAtPos(x, y + 1, &tempCollisionsToCheck);
//    if (isOnGround) printf("On the ground!\n");
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
