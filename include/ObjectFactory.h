#pragma once
#include <functional>
#include "Object.h"
#include <string>
#include "Room.h"

// See
// https://stackoverflow.com/questions/5450742/is-it-possible-in-c-to-loop-over-all-subclasses-of-an-abstract-class
// https://dorodnic.com/blog/2014/03/29/abstract-factory/

class Object;
class Room;

class ObjectFactory
{
    public:
        static ObjectFactory& GetObjectFactory();    // SINGLETON!!!! BANZAI!!!

        Object* Build(std::string const& key, int gx, int gy, Room* rm) const;

    private:

};
