#pragma once

#include "Object.h"
#include "Shape/Quad.h"

class TestGameObj : public Object
{
    public:
        TestGameObj(int gx, int gy, Room* rm);
        virtual ~TestGameObj();

        void Update();
        void Render();

        bool IsColliding(BoundBox* box) { }

    protected:

    private:
    	Quad* image;

    	float hsp = 0, vsp = 0;
    	int ____i = 0;

//    	static const bool registeredObj = ObjectFactory.GetObjectFactory().Register("Bar", objectBuilder<TestGameObj>);
};
