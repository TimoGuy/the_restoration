#pragma once
#ifdef __unix__
#include "Object.h"
#include "Quad.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../Object.h"
#include "../Shape/Quad.h"
#endif

#include <vector>

//class Room;

class Entity : public Object
{
public:
	Entity(int gx, int gy, TestRoom* rm, bool isSolid=false);
	~Entity();

	virtual void YouLose(Entity* accordingToMe) = 0;

	float GetHsp();

protected:
	float hsp, vsp;
	Quad* image;


	void SeeNeighborCollisionObjects(float centerX, float centerY, std::vector<Object*>& neighboringCollisions);
	void UpdateGroundCollisionVelocity(float & hspeed, float & vspeed, float width, float height, std::vector<Object*>* collisionsToCheck);

	bool CollideAtPos(float futX, float futY, float width, float height, std::vector<Object*>* collisionsToCheck, bool onlySolids);
	bool CollideAtPos(float futX, float futY, float width, float height, std::vector<Object*>* collisionsToCheck, std::vector<Object*>& returnObjsCollided, bool onlySolids);
};

