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

        // States
		int numJumps = 0;
		int maxJumps = 2;	// Default: 1 for only 1 jump eh.
		bool wasJumpBtnAlreadyPressed = false;
        
        float nerfer = 0;	// DEBUGGING VARIABLE
        float highestHsp = 0;

        float startX = 0, startY = 0;
};
