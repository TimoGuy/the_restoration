#pragma once
#ifdef __unix__
#include "Object.h"
#include "Room.h"
#elif defined(_WIN32) || defined(WIN32)
#include "Object.h"
#include "Rooms\Room.h"
#endif

#include <functional>
#include <string>

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
