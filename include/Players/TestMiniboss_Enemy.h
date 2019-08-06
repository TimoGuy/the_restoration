#pragma once

#include "Entity.h"


class TestMiniboss_Enemy :
	public Entity
{
public:
	TestMiniboss_Enemy(int gx, int gy, TestRoom* rm);
	virtual ~TestMiniboss_Enemy();

	void Update();
	void Render();

	bool IsColliding(BoundBox* box);

	void YouLose(Entity* accordingToMe);

private:
    float life;
	Entity* targetEnt;	// Usually this is the player eh

	bool onGround;

	void FindTargetEntity();
	bool IsTargetInBounds();		// Find if in line of sight eh.
};

