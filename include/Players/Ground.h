#pragma once
#include <Object.h>


class Ground : public Object
{
    public:
        Ground(int gx, int gy);
        virtual ~Ground();

        void Update() {}
        void Render() {}

    protected:

    private:
};
