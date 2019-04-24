#pragma once
#include <functional>
#include "Object.h"
#include <string>

// See
// https://stackoverflow.com/questions/5450742/is-it-possible-in-c-to-loop-over-all-subclasses-of-an-abstract-class
// https://dorodnic.com/blog/2014/03/29/abstract-factory/

class Object;

class ObjectFactory
{
    public:
        static ObjectFactory& GetObjectFactory();    // SINGLETON!!!! BANZAI!!!

        Object* Build(std::string const& key) const;

    private:

};
