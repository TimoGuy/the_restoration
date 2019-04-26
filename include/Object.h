#pragma once
#include "ObjectFactory.h"
#include "Room.h"
#include "Quad.h"


struct BoundBox
{
    float x;
    float y;
    float width;
    float height;
};

class Room;








// Every object should be able to be inputted
// into the level editor (mspaint)
//
// In order to add an object, add its id in
// ObjectFactory.cpp into
// 1. the enum 'StringValue',
// 2. inside the 'ObjectFactory::GetObjectFactory()',
// 3. the enum entry into the big switch in the
//      'ObjectFactory::Build(...)' method
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
