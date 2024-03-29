#pragma once

#if defined(__unix__) || defined(__APPLE__)
#include "Base.h"
#elif defined(_WIN32) || defined(WIN32)
#include "Base.h"
#endif

class ObjectFactory;
class Quad;


#include <vector>


struct BoundBox
{
    float x;
    float y;
	float prevx;
	float prevy;
    float width;
    float height;
};

class TestRoom;








// Every object should be able to be inputted
// into the level editor (mspaint)
//
// In order to add an object, add its id in
// ObjectFactory.cpp into
// 1. the enum 'StringValue',
// 2. inside the 'ObjectFactory::GetObjectFactory()',
// 3. the enum entry into the big switch in the
//      'ObjectFactory::Build(...)' method
class Object : public Base
{
    public:
        Object(int gx, int gy, TestRoom* rm, bool isSolid);
        virtual ~Object();

        virtual void Update() = 0;
        virtual void Render() = 0;

        void SetGridCoords(int gx, int gy);

        float getX() { return x; }
        float getY() { return y; }
		bool IsSolid() { return _isSolid; }

        virtual bool IsColliding(BoundBox* box) = 0;

    protected:
        float x, y;
        TestRoom* room;

		bool _isSolid;

    private:
};
