#include "CutsceneObject.h"
#include <sstream>
#include <cmath>

CutsceneObject::CutsceneObject(int x, int y, CutsceneSprite* image)
{
    _x = x;
    _y = y;
    dx = dy = 0;
    _image = image;
    wantToEnd = false;
}

CutsceneObject::~CutsceneObject()
{
    delete _image;
}


void CutsceneObject::Update(int ticks)
{
    dx = dy = 0;    // For good luck

    // Run all of the registered functions eh!
    for (int i = 0; i < functions.size(); i++)
    {
        // Check if within the time bounds! (ticks)
        if (ticks >= functions.at(i).startTick &&
            ticks <= functions.at(i).endTick)
        {
            // Run the function!!!! (don't mess w/ my code!)
            (this->*(functions.at(i).func))
                (ticks,
                functions.at(i).startTick,
                functions.at(i).endTick,
                functions.at(i).params);
        }
    }
}





bool CutsceneObject::Render(int ticks)
{
    if (wantToEnd)
        return false;

    _image->Render(_x + dx, _y + dy, ticks);
    return true;
}









void CutsceneObject::RegisterFunction
    (int startTick, int endTick, std::string func, std::string params)
{
    // Use a switch to see which func should use
    void (CutsceneObject::*newFunc)(int, int, int, std::string);

    // Checks!!!
    if (func == std::string("move"))
        newFunc = &CutsceneObject::Move;
    else if (func == std::string("snap"))
        newFunc = &CutsceneObject::SetCoords;
    else if (func == std::string("wiggle-x"))
        newFunc = &CutsceneObject::WiggleX;
    else if (func == std::string("end"))
        newFunc = &CutsceneObject::End;

    // Error
    else
    {
        printf("\tERROR: could not find function \"%s\"\n", func.c_str());
        return;
    }





    // Add the params w/ the function

    // Store the params into the struct
    FuncPlusParams fpp = {
        newFunc,       // Just the very function we inserted!
        startTick,              // Starting time
        endTick,                // Ending time
        params,                  // My goodness, can't you read??
    };

    // And store the bomber package!!!!
    functions.push_back(fpp);




    // Just for fun!
    printf("\tFunc registered!\n");
}


// The very many cutscene object actions~~~~!!!!!!
/////////////////
void CutsceneObject::Move(int currentTick, int startTick, int endTick, std::string params)
{
    // Get the x and y coords (out of params) first of all
    int toX, toY;
    {
        std::string token;
        std::istringstream tokenStream(params);
        int times = 0;
        while (std::getline(tokenStream, token, ',') &&
            times <= 1)
        {
            if (times == 0)     // Width
            {
                std::istringstream(token) >> toX;
            }
            else if (times == 1)    // Height
            {
                std::istringstream(token) >> toY;
            }

            times++;
        }
    }



    // See if last action... if so, set the x and y to this too!
    if (currentTick == endTick)
    {
        _x = toX;
        _y = toY;
        return;
    }

    // Find the distances
    float ratio_booboo =
        (float)(currentTick - startTick) / (float)(endTick - startTick);

    // Mix it into the delta vars!!!
    dx += (toX - _x) * ratio_booboo;
    dy += (toY - _y) * ratio_booboo;
}



void CutsceneObject::SetCoords(int currentTick, int startTick, int endTick, std::string params)
{
    // Get the x and y coords (out of params) first of all
    int toX, toY;
    {
        std::string token;
        std::istringstream tokenStream(params);
        int times = 0;
        while (std::getline(tokenStream, token, ',') &&
            times <= 1)
        {
            if (times == 0)     // Width
            {
                std::istringstream(token) >> toX;
            }
            else if (times == 1)    // Height
            {
                std::istringstream(token) >> toY;
            }

            times++;
        }
    }

    _x = toX;
    _y = toY;
}



void CutsceneObject::WiggleX(int currentTick, int startTick, int endTick, std::string params)
{
    int variance;
    int backAndForth;

    {
        std::string token;
        std::istringstream tokenStream(params);

        std::getline(tokenStream, token, '\t');     // Check variance
        std::istringstream(token) >> variance;
        std::getline(tokenStream, token, '\t');     // Check back and forth kaisuu
        std::istringstream(token) >> backAndForth;
    }



    // Wiggle!

    int distance = endTick - startTick;
    float bubun = (currentTick - startTick) / (float)distance;

    float angle = bubun * 2.0f * M_PI * backAndForth;
    dx += std::sin(angle) * variance;
}



void CutsceneObject::End(int currentTick, int startTick, int endTick, std::string params)
{
    wantToEnd = true;
}
