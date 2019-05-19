#pragma once

#ifdef __unix__
#include "Object.h"
#elif defined(_WIN32) || defined(WIN32)
//#include "../Object.h"
//#include "../Players/Entity.h"
#endif
#include <vector>

class Object;
class Entity;

class Room
{
    public:
        Room();
        virtual ~Room();

		Object** getCollisionMap() { return collisionMap; }
		std::vector<Entity*>* getEntityList() { return &entityList; }
        int getGWidth() { return gWidth; }
        int getGHeight() { return gHeight; }

        virtual void Update() = 0;
        virtual void Render() = 0;

    protected:
        Object** collisionMap;      // This will be an array
		std::vector<Entity*> entityList;	// Stores all the entities inside of the room!
        int gWidth, gHeight;        // The grid's width and height

    private:
};
