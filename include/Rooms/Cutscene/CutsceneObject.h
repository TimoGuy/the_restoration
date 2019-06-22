#pragma once

#include "CutsceneSprite.h"

#include <string>
#include <vector>

struct FuncPlusParams;

class CutsceneObject
{
    public:
        CutsceneObject(int x, int y, int spriteId, Cutscene* myCutscene);
        virtual ~CutsceneObject();

        void Update(int ticks);
        void Render(int ticks);

        // An IO-friendly function to create these objects
        void RegisterFunction(int startTick, int endTick, std::string func, std::string params);

    protected:

    private:
        Cutscene* cutscene;     // So can load the sprite list toka.

    	CutsceneSprite* _image;        // So that animation is supported..
        int _x, _y;     // Only set these when an action has ended!
        int dx, dy;     // Any function can play w/ these...

        float _angle;    // This is the rotation eh!
        float dAngle;

        float _scale;    // This is the scaling!
        float dScale;
        std::vector<FuncPlusParams> functions;


        float sprAlpha;




        // Functions that a cutscene obj may use!
        //
        // NOTE: they are only executed when currentTick is [startTick, endTick)
        //
        // Needs to give the current tick, and the start-end stuff eh!
        // for every func., okay???
        void Move(int currentTick, int startTick, int endTick, std::string params);
        void MoveLerp(int currentTick, int startTick, int endTick, std::string params);
        void Rotate(int currentTick, int startTick, int endTick, std::string params);
        void Scale(int currentTick, int startTick, int endTick, std::string params);
        void SetCoords(int currentTick, int startTick, int endTick, std::string params);
        void WiggleX(int currentTick, int startTick, int endTick, std::string params);
        void WiggleY(int currentTick, int startTick, int endTick, std::string params);
        void ChangeSprite(int currentTick, int startTick, int endTick, std::string params);
        void FadeIn(int currentTick, int startTick, int endTick, std::string params);
        void FadeOut(int currentTick, int startTick, int endTick, std::string params);
        void DeleteMe(int currentTick, int startTick, int endTick, std::string params);
        void ExitCutscene(int currentTick, int startTick, int endTick, std::string params);
};









// Implement it!
struct FuncPlusParams
{
    void (CutsceneObject::*func)(int, int, int, std::string);       // Needs to be under CutsceneObject:: so that it can capture a member function!
    int startTick;
    int endTick;
    std::string params;
};

