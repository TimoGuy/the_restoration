#pragma once
#include "ObjectFactory.h"

// Every object should be able to be inputted
// into the level editor (mspaint)

class Object
{
    public:
        Object(int gx, int gy);
        virtual ~Object();

        virtual void Update() = 0;
        virtual void Render() = 0;

        float getX() { return x; }
        float getY() { return y; }

    protected:
        float x, y;

    private:
};

/*

ONEGAI!!!!!!!
Please, for the sake of the level editor IO,
Add any objects you make into the factory so
it can read it!

static const bool registeredBar = GetFooFactory().Register("Bar", fooBuilder<Bar>);


*/
