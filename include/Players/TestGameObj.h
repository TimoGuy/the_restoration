#pragma once

#include "Object.h"
#include "Shape/Quad.h"

class TestGameObj : public Object
{
    public:
        TestGameObj();
        virtual ~TestGameObj();

        void Update();
        void Render();

    protected:

    private:
    	Quad* image;
    	float x, y;

//    	static const bool registeredObj = ObjectFactory.GetObjectFactory().Register("Bar", objectBuilder<TestGameObj>);
};
