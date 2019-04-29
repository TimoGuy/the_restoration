#pragma once

#include "Object.h"
#include "Shape/Quad.h"
#include <vector>

class TestGameObj : public Object
{
    public:
        TestGameObj(int gx, int gy, Room* rm);
        virtual ~TestGameObj();

        void Update();
        void Render();

        bool IsColliding(BoundBox* box) { }

    protected:

    private:
    	Quad* image;

    	float hsp = 0, vsp = 0;
    	int ____i = 0;

        bool CollideAtPos(float futX, float futY, std::vector<Object*>* collisionsToCheck);
        bool CollideAtPos(float futX, float futY, std::vector<Object*>* collisionsToCheck, Object*& returnObjCollided);


        // States
        bool isOnGround = false;
        float nerfer = 0;
        float highestHsp = 0;

        float startX = 0, startY = 0;
};
