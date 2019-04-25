#pragma once
#include <Object.h>
#include "Quad.h"

class Ground : public Object
{
    public:
        Ground(int gx, int gy, Room* rm);
        virtual ~Ground();

        void Update() {}
        void Render();

        bool IsColliding(BoundBox* box);

    protected:

    private:
    	Quad* image;
};
