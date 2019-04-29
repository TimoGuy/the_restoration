#pragma once
#ifdef __unix__
#include <Object.h>
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Object.h"
#endif

class Exit : public Object
{
    public:
        Exit(int gx, int gy, Room* rm);
        virtual ~Exit();

        void Update() {}
        void Render() {}

		bool IsColliding(BoundBox* box) { return false; }

    protected:

    private:
};
