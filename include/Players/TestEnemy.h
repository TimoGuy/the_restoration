#pragma once
#include "Entity.h"

class TestEnemy :
	public Entity
{
public:
	TestEnemy(int gx, int gy, Room* rm);
	virtual ~TestEnemy();

	void Update();
	void Render();

	bool IsColliding(BoundBox* box);
};

