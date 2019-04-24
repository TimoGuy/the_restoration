#pragma once

#include "Object.h"
#include "Shape/Quad.h"

class TestGameObj : public Object
{
    public:
        TestGameObj(int gx, int gy);
        virtual ~TestGameObj();

        void Update();
        void Render();

    protected:

    private:
    	Quad* image;

//    	static const bool registeredObj = ObjectFactory.GetObjectFactory().Register("Bar", objectBuilder<TestGameObj>);
};
