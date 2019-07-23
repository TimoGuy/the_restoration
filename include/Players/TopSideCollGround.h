#pragma once

#include "Ground.h"


class TopSideCollGround :
	public Ground
{
public:
	TopSideCollGround(int gx, int gy, TestRoom* rm);
	~TopSideCollGround();

	bool IsColliding(BoundBox* box) override;
};

