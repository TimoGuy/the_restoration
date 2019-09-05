#pragma once

#include "Entity.h"
#if defined(__unix__) || defined(__APPLE__)
#include "json/json.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../json/json.h"
#endif

class SpriteSheetIO;


class TestEnemy :
	public Entity
{
public:
	TestEnemy(int gx, int gy, TestRoom* rm);
	TestEnemy(int gx, int gy, TestRoom* rm, std::string fname);
	virtual ~TestEnemy();

	void Update();
	void Render();

	bool IsColliding(BoundBox* box);

	void YouLose(Entity* accordingToMe);

	bool IsHazardous();

private:
	void ProcessAction(std::string actionName, Json::Value actionArgs, bool isHazardous);

	void FindTargetEntity();
	bool IsTargetInBounds(float gridRadius);

	BoundBox* myBoundBox;

	Entity* targetEnt;
    float life;

	int currentAction;
	int attackPhase;
	int attackPhase_tick;
	bool requestToNextAttackPhase;
	bool isHazardous;

	bool isFacingLeft;		// If not then you're facing right eh

	Json::Value props;
	std::string animAction;
	SpriteSheetIO* sprSheet;

	bool isDying;
	int ticksDying;
};

