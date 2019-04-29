#pragma once
#include "Ground.h"
class TopSideCollGround :
	public Ground
{
public:
	TopSideCollGround(int gx, int gy, Room* rm);
	~TopSideCollGround();

	bool IsColliding(BoundBox* box) override;
};

