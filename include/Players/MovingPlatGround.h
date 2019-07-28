#pragma once

#if defined(__unix__) || defined(__APPLE__)
#include "Object.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../Object.h"
#endif

struct BoundBox;
class Quad;


#include <vector>

class MovingPlatGround : public Object
{
    public:
		MovingPlatGround(int gx, int gy, TestRoom* rm);
        virtual ~MovingPlatGround();

		void Update();
        void Render();

        bool IsColliding(BoundBox* box);

		void Extend1BlockToTheRight(int newGx, int newGy);

		int GetHsp() { return hsp; }
		int GetVsp() { return vsp; }

    protected:
    	Quad* image;

    private:
		void RecalcMyselfToCollisionMap();
		int prevX, prevY;
		std::vector<int> collCoords;	// List of collision stops this object is registered at!

		// Movement velocity data to tell other objects
		int hsp, vsp;
};
