#pragma once
#ifdef __unix__
#include "Entity.h"
#include "Shape/Quad.h"
#elif defined(_WIN32) || defined(WIN32)
#include "Entity.h"
#include "../Shape/Quad.h"
#endif

#include <vector>

class TestGameObj : public Entity
{
    public:
        TestGameObj(int gx, int gy, TestRoom* rm);
        virtual ~TestGameObj();

        void Update();
        void Render();

        void SetGridCoords(int gx, int gy);     // This func is getting overriden eh
		void UpdateStartCoords();

		bool IsColliding(BoundBox* box) { return false; }

		void YouLose(Entity* accordingToMe);

    protected:

    private:
        Quad* pf;

        // States
		int numJumps = 0;
		int maxJumps = 2;	// Default: 1 for only 1 jump eh.
		bool wasJumpBtnAlreadyPressed = false;

		int framesOfInvincibility = 0;      // This counts down until 0, wherewith you are not invincible anymore

        float nerfer = 0;	// DEBUGGING VARIABLE
        float highestHsp = 0;

        float startX = 0, startY = 0;

		int outHsp, outVsp;		// Outside forces will be int's, really!
};
