#pragma once
#ifdef __unix__
//#include "Object.h"
//#include "Room.h"
#elif defined(_WIN32) || defined(WIN32)
//#include "Object.h"
//#include "Rooms\Room.h"
#endif

#include <functional>
#include <string>
#include <vector>

// See
// https://stackoverflow.com/questions/5450742/is-it-possible-in-c-to-loop-over-all-subclasses-of-an-abstract-class
// https://dorodnic.com/blog/2014/03/29/abstract-factory/

class Object;
class Room;

class ObjectFactory
{
    public:
        static ObjectFactory& GetObjectFactory();    // SINGLETON!!!! BANZAI!!!

        Object* Build(std::string const& key, std::vector<std::string>* rmParams, int gx, int gy, Room* rm) const;

    private:
		// This is only for the slant creation code...
		// Basically, if the last added thing is the right
		// kind of slant, then continue the slant
		static int previousStringValue;		// The enum values are already pointing to int's anyways...
		static Object* previousObj;		// You're going to edit this slant if needed...
		// TODO: for extending the slant, you gotta know if the pixel for the slant isn't just wrapping around the screen!!!!! Or else there'll be some problems
};
