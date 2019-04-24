#pragma once
#include <Object.h>
#include "Quad.h"

class Ground : public Object
{
    public:
        Ground(int gx, int gy);
        virtual ~Ground();

        void Update() {}
        void Render();

    protected:

    private:
    	Quad* image;
};
