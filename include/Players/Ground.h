#pragma once

#ifdef __unix__
#include "Object.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../Object.h"
#endif

struct BoundBox;
class TestRoom;
class Quad;


class Ground : public Object
{
    public:
        Ground(int gx, int gy, TestRoom* rm);
        virtual ~Ground();

        void Update() {}
        void Render();

        bool IsColliding(BoundBox* box);

    protected:
    	Quad* image;

    private:
};
