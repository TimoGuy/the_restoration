#pragma once
#include <Object.h>


class Exit : public Object
{
    public:
        Exit(int gx, int gy);
        virtual ~Exit();

        void Update() {}
        void Render() {}

    protected:

    private:
};
