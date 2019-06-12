#pragma once

#ifdef __unix__
#include <Object.h>
#elif defined(_WIN32) || defined(WIN32)
#include "../Object.h"
#endif

class Slant : public Object
{
    public:
        Slant(int gx, int gy, TestRoom* rm, bool isLeft);
        virtual ~Slant();

        void Update() {}
		void Render();

		bool IsColliding(BoundBox* otherBox);

		void Extend1BlockToTheRight(int newGx, int newGy);

    protected:

    private:
#define SLANT_PROPERTIES_MAX_BLOCKS 4
		struct Point { float x, y; };
		struct Box { Point min, max; };

		int originalGy;
		bool _isLeft;
		int _blocks;		// The max should be 4 (aka 1, 2, 3, and 4 are your options eh)
		Point _start, _end;		// Always have the _start be left of the _end...
};
