#pragma once
#include "ObjectFactory.h"
#include "Room.h"
#include "Quad.h"

// Every object should be able to be inputted
// into the level editor (mspaint)

struct BoundBox
{
    float x;
    float y;
    float width;
    float height;
};

class Room;

class Object
{
    public:
        Object(int gx, int gy, Room* rm);
        virtual ~Object();

        virtual void Update() = 0;
        virtual void Render() = 0;

        float getX() { return x; }
        float getY() { return y; }

        virtual bool IsColliding(BoundBox* box) = 0;

    protected:
        float x, y;
        Room* room;

    private:
};

/*

ONEGAI!!!!!!!
Please, for the sake of the level editor IO,
Add any objects you make into the factory so
it can read it!

static const bool registeredBar = GetFooFactory().Register("Bar", fooBuilder<Bar>);


*/
