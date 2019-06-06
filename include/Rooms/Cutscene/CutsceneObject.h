#pragma once

#include "CutsceneSprite.h"

#include <string>
#include <vector>

class CutsceneObject
{
    public:
        CutsceneObject(int x, int y, CutsceneSprite* image);
        virtual ~CutsceneObject();

        void Update(int ticks);
        void Render(int ticks);

        // An IO-friendly function to create these objects
        void RegisterFunction(int startTick, int endTick, std::string func, std::string params);

    protected:

    private:
    	CutsceneSprite* _image;        // So that animation is supported..
        int _x, _y;
        std::vector<void (*)(int, int, std::string)> functions;

        // Functions that a cutscene obj may use!
        // Needs to be (int startTick, std::string params)
        // for every func., okay???
        void Move(int startTick, int endTick, std::string params);
        void SetCoords(int startTick, int endTick, std::string params);
};
