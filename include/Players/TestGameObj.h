#pragma once
#ifdef __unix__
#include "Object.h"
#include "Shape/Quad.h"
#elif defined(_WIN32) || defined(WIN32)
#include "Entity.h"
#include "../Shape/Quad.h"
#endif

#include <vector>

class TestGameObj : public Entity
{
    public:
        TestGameObj(int gx, int gy, Room* rm);
        virtual ~TestGameObj();

        void Update();
        void Render();

		bool IsColliding(BoundBox* box) { return false; }

    protected:

    private:
    	int ____i = 0;


        // States
        bool isOnGround = false;
        float nerfer = 0;
        float highestHsp = 0;

        float startX = 0, startY = 0;
};
