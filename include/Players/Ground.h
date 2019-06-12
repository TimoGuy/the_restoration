#pragma once
#ifdef __unix__
#include <Object.h>
#include "Quad.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../Object.h"
#include "../Shape/Quad.h"
#endif

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
