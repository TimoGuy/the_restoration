#pragma once
#include "Object.h"
#include "Quad.h"


class Hazard : public Object
{
    public:
        Hazard(int gx, int gy, Room* rm);
        virtual ~Hazard();

        void Update() {}
        void Render();

        bool IsColliding(BoundBox* box);

    protected:

    private:
        Quad* image;
};
