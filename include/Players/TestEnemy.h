#pragma once
#include "Entity.h"

class TestEnemy :
	public Entity
{
public:
	TestEnemy(int gx, int gy, TestRoom* rm);
	virtual ~TestEnemy();

	void Update();
	void Render();

	bool IsColliding(BoundBox* box);

	void YouLose(Entity* accordingToMe);

private:
    float life;
};

