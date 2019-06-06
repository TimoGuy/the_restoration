#include "CutsceneObject.h"

CutsceneObject::CutsceneObject(int x, int y, CutsceneSprite* image)
{
    _x = x;
    _y = y;
    _image = image;
}

CutsceneObject::~CutsceneObject()
{
    delete _image;
}


void CutsceneObject::Update(int ticks)
{
    // TODO: run all of the registered functions eh!
}


void CutsceneObject::Render(int ticks)
{
    _image->Render(_x, _y, ticks);
}




void CutsceneObject::RegisterFunction(int startTick, int endTick, std::string func, std::string params)
{
    printf("\tFunc registered!\n");
}
