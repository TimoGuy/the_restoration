#pragma once
#if defined(__unix__) || defined(__APPLE__)
#elif defined(_WIN32) || defined(WIN32)
#endif

#include <functional>
#include <string>
#include <vector>

namespace Json {
	class Value;
}

// See
// https://stackoverflow.com/questions/5450742/is-it-possible-in-c-to-loop-over-all-subclasses-of-an-abstract-class
// https://dorodnic.com/blog/2014/03/29/abstract-factory/

class Object;
class TestRoom;

class ObjectFactory
{
    public:
        static ObjectFactory& GetObjectFactory();    // SINGLETON!!!! BANZAI!!!

        Object* Build(std::string const& key, int gx, int gy, TestRoom* rm, Json::Value& rmOptions);

    private:
        ObjectFactory();

		// This is only for the slant creation code...
		// Basically, if the last added thing is the right
		// kind of slant, then continue the slant
		int previousStringValue;		// The enum values are already pointing to int's anyways...
		Object* previousObj;		// You're going to edit this slant if needed...
};
