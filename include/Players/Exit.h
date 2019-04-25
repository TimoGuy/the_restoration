#pragma once
#include <Object.h>


class Exit : public Object
{
    public:
        Exit(int gx, int gy, Room* rm);
        virtual ~Exit();

        void Update() {}
        void Render() {}

        bool IsColliding(BoundBox* box) { }

    protected:

    private:
};
