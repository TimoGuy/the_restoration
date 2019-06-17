#pragma once
#include "Object.h"
#include "Quad.h"
#include <vector>

//class Room;

class Entity : public Object
{
public:
	Entity(int gx, int gy, TestRoom* rm);
	~Entity();

	virtual void YouLose(Entity* accordingToMe) = 0;

protected:
	float hsp, vsp;
	Quad* image;


	void SeeNeighborCollisionObjects(float centerX, float centerY, std::vector<Object*>& neighboringCollisions);
	void UpdateGroundCollisionVelocity(float & hspeed, float & vspeed, float width, float height, std::vector<Object*>* collisionsToCheck);

	bool CollideAtPos(float futX, float futY, float width, float height, std::vector<Object*>* collisionsToCheck, bool onlySolids);
	bool CollideAtPos(float futX, float futY, float width, float height, std::vector<Object*>* collisionsToCheck, std::vector<Object*>& returnObjsCollided, bool onlySolids);
};

