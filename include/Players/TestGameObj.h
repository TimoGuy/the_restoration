#pragma once

#if defined(__unix__) || defined(__APPLE__)
#include "Entity.h"
#elif defined(_WIN32) || defined(WIN32)
#include "Entity.h"
#endif

class Textbox;
class Quad;
class SpriteSheetIO;


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

		int GetNumJumps();

    protected:

    private:
        std::vector<Textbox*> pf;
        Quad* startCoords;

        SpriteSheetIO* sprSheet;

        bool isMidair;
        int lifeRechargeTicks;

        // States
        bool IsUsingSword();
        int swordTicksLeft;

		int numJumps = 0;
		bool wasJumpBtnAlreadyPressed = false;

		int framesOfInvincibility = 0;      // This counts down until 0, wherewith you are not invincible anymore

        float nerfer = 0;	// DEBUGGING VARIABLE
        float highestHsp = 0;

        float startX = 0, startY = 0;

		int outHsp, outVsp;		// Outside forces will be int's, really!

		// For 'rocket jump' component
		bool isDoingRocketJump;
		bool _freshRocketJump;				// This is only true on the first tick of a rocket jump (like ticket to enter)
		int rjInputActivationPhase;			// 0 is nothing, 1 is holding a direction, 2 is go into deadzone, 3 is back to prev. direction (like a double tap!)
		int rjInputDirection;				// Like the cardinal directions since this is only 4 directions eh
		int rjTicksInDeadzone;
		int rjTicksDoingRocketJump;
		float rjStoredHsp, rjStoredVsp;
};
