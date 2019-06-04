#pragma once

#include "Quad.h"

#include <string>
#include <vector>

class CutsceneObject
{
    public:
        CutsceneObject(int x, int y, Quad* image);
        virtual ~CutsceneObject();

        void Update(int ticks);
        void Render();

        // TODO: an IO function to create these objects
        void RegisterFunction(int startTick, std::string func, std::string params);

    protected:

    private:
    	Quad* image;        // Really this should be a SPRITE so that animation is supported..
        int x, y;
        std::vector<void (*)(int, int, std::string)> functions;

        // Functions that a cutscene obj may use!
        // Needs to be (int startTick, std::string params)
        // for every func., okay???
        void Move(int startTick, int endTick, std::string params);
        void SetCoords(int startTick, int endTick, std::string params);
};
