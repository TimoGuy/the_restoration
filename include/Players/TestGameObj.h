#pragma once
#ifdef __unix__
#include "Object.h"
#include "Shape/Quad.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../Object.h"
#include "../Shape/Quad.h"
#endif

#include <vector>

class TestGameObj : public Object
{
    public:
        TestGameObj(int gx, int gy, Room* rm);
        virtual ~TestGameObj();

        void Update();
        void Render();

		bool IsColliding(BoundBox* box) { return false; }

    protected:

    private:
    	Quad* image;

    	float hsp = 0, vsp = 0;
    	int ____i = 0;

        bool CollideAtPos(float futX, float futY, std::vector<Object*>* collisionsToCheck);

        // States
        bool isOnGround = false;
        float nerfer = 0;
        float highestHsp = 0;
};
