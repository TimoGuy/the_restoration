#ifdef __unix__
#include "Entity.h"
#include "defs.h"

#include "Slant.h"
#elif defined(_WIN32) || defined(WIN32)
#include "..\..\include\Players\Entity.h"
#include "../../include/defs.h"

#include "../../include/Players/Slant.h"		// Hope I can include this!!!
#endif

#include <cmath>


Entity::Entity(int gx, int gy, Room* rm)
	: Object(gx, gy, rm, false)
{
	hsp = vsp = 0;
	rm->getEntityList()->push_back(this);	// Adds itself to the entity list of the room
}


Entity::~Entity()
{
	//dtor
	delete image;
}












void Entity::SeeNeighborCollisionObjects(float centerX, float centerY, std::vector<Object*>& neighboringCollisions)
{
	int gx = centerX / GRID_SIZE,     // gx and gy mean that this is where
		gy = centerY / GRID_SIZE;     // the center of the obj lies in the collision grid

									  // It's probably good to check the current position (within the collision chart) and the 8 around it!
	int ngx, ngy;
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
				neighboringCollisions.push_back(tempColl);
			}
		}
	}
}


#define ENTITY_MAX_CLIMB_HEIGHT 35.0f
// Well, really the thing above should be the maxest max_hsp, that way you can still go up the slant
// Or you could try lowering this and then move the testing cursor closer (x-wise) to the prev. position
// And if there's a free space, then SLOW the player down! (That's an idea, but wait wait wait to implement...)

void Entity::UpdateGroundCollisionVelocity(float& hspeed, float& vspeed, float width, float height, std::vector<Object*>* collisionsToCheck)
{
	// THIS IS COLLISION W/ GROUND SECTION


	// Try updating x (from hsp)				// EDIT: we'll add the ability to see if collided into a slant or not! This only happens if you're RUBBING into it though!
	int tHsp = ceil(abs(hspeed));
	std::vector<Object*> tempCollisions;
	if (!CollideAtPos(x + tHsp * copysignf(1.0f, hspeed), y, width, height, collisionsToCheck, tempCollisions, true))
	{
		// It's safe!
		x += tHsp * copysignf(1.0f, hspeed);
	}
	else if (tHsp > 0)
	{
		bool successClimb = false;

#pragma region Slant_climbing!!!
		for (int i = 0; i < tempCollisions.size(); i++)
		{
			if (dynamic_cast<Slant*>(tempCollisions.at(i)) != NULL)
			{
				// So now we know, there's a slant! Let's see if we can climb up this thing!!!
				for (int offY = -1; offY >= -ENTITY_MAX_CLIMB_HEIGHT; offY--)
				{
					// Make sure to check all the area in case if there're more slopes!!!
					if (!CollideAtPos(x + tHsp * copysignf(1.0f, hspeed), y + offY, width, height, collisionsToCheck, true))
					{
						// Success! Make vspeed as high as it needs to be!!!!!!!
						//vsp += offY;
						vsp += offY / 4.25f;
						y += offY;
						hsp = hsp * 0.9985f;

						// It's safe!
						x += tHsp * copysignf(1.0f, hspeed);
						successClimb = true;
						break;
					}
				}

				// Break out of big loop too.
				// I'm sorry, it's just easier this way...
				break;
			}
		}
#pragma endregion

		if (!successClimb)
		{
			// Ran into something... let's see!
			for (tHsp -= 1; tHsp > 0; tHsp--)
			{
				// Roll back 1 at a time and see if no collision
				int newX = round(x) + tHsp * copysignf(1.0f, hspeed);
				if (!CollideAtPos(newX, y, width, height, collisionsToCheck, true))
				{
					// Update!
					x = newX;
					break;
				}
			}

			// Collided so let's get this thing stopping...
			hspeed = 0;
		}
	}




	// Try updating y (from vsp)										// NOTE: the value 'vsp' could be modified by the ramp climbing back here!
	int tVsp = ceil(abs(vspeed));
	if (!CollideAtPos(x, y + tVsp * copysignf(1.0f, vspeed), width, height, collisionsToCheck, true))
	{
		// It's safe!
		y += tVsp * copysignf(1.0f, vspeed);
	}
	else if (tVsp > 0)
	{
		// Ran into something... let's see!
		for (tVsp -= 1; tVsp > 0; tVsp--)
		{
			// Roll back 1 at a time and see if no collision
			int newY = round(y) + tVsp * copysignf(1.0f, vspeed);
			if (!CollideAtPos(x, newY, width, height, collisionsToCheck, true))
			{
				// Update!
				y = newY;
				break;
			}
		}

		// Collided so let's get this thing stopping...
		vspeed = 0;
	}
}









bool Entity::CollideAtPos(float futX, float futY, float width, float height, std::vector<Object*>* collisionsToCheck, bool onlySolids)
{
	std::vector<Object*> useless;		// So this is just... well, useless eh!
	return CollideAtPos(futX, futY, width, height, collisionsToCheck, useless, onlySolids);
}

bool Entity::CollideAtPos(float futX, float futY, float width, float height, std::vector<Object*>* collisionsToCheck, std::vector<Object*>& returnObjsCollided, bool onlySolids)
{
	// And then see if collided!
	bool collided = false;
	for (int i = 0; i < collisionsToCheck->size(); i++)
	{
		BoundBox b = { futX, futY, x, y, width, height };
		if (collisionsToCheck->at(i)->IsColliding(&b) &&
			(!onlySolids || collisionsToCheck->at(i)->IsSolid()))
		{
			// Collided!
			returnObjsCollided.push_back(collisionsToCheck->at(i));
			collided = true;
		}
	}
	return collided;
}

