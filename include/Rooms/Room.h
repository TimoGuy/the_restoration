#pragma once

#include "Object.h"

class Object;

class Room// : public Object
{
    public:
        Room();
        virtual ~Room();

        Object** getCollisionMap() { return collisionMap; }
        int getGWidth() { return gWidth; }
        int getGHeight() { return gHeight; }

        virtual void Update() = 0;
        virtual void Render() = 0;

    protected:
        Object** collisionMap;      // This will be an array
        int gWidth, gHeight;        // The grid's width and height

    private:
};
