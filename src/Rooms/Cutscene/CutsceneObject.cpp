#if defined(__unix__) || defined(__APPLE__)
#include "Cutscene.h"       // That way don't have to load in the header file!
#include "CutsceneSprite.h"
#include "CutsceneObject.h"
#include "TestRoom.h"
#include <SDL2/SDL_opengl.h>
#elif defined(_WIN32) || defined(WIN32)
#include "../../../include/Rooms/Cutscene/Cutscene.h"
#include "../../../include/Rooms/Cutscene/CutsceneSprite.h"
#include "../../../include/Rooms/Cutscene/CutsceneObject.h"
#include "../../../include/Rooms/TestRoom.h"
#include <SDL_opengl.h>
#endif

#include <sstream>
#include <cmath>
#include <cstring>

CutsceneObject::CutsceneObject(int x, int y, int spriteId, Cutscene* myCutscene)
{
    _x = (float)x;
    _y = (float)y;
    dx = dy = 0;

    _angle = 0;
    dAngle = 0;

    _scale = 1;
    dScale = 0;

    cutscene = myCutscene;

    _image = cutscene->GetSpriteByID(spriteId);
    sprAlpha = 1;
}

CutsceneObject::~CutsceneObject()
{
    if (_image != NULL)
        delete _image;
}


void CutsceneObject::Update(int ticks)
{
    dx = dy = 0;    // For good luck
    dAngle = 0;
    dScale = 0;

    // Run all of the registered functions eh!
    for (unsigned int i = 0; i < functions.size(); i++)
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





void CutsceneObject::Render(int ticks)
{
	if (_image == NULL) return;

    glTranslatef(_x + dx, _y + dy, 0);
    glRotatef(_angle + dAngle, 0, 0, 1);
    glScalef(_scale + dScale, _scale + dScale, 1);

    _image->Render(0, 0, ticks, sprAlpha);          // Transforms are already applied eh.

    glScalef(1.0f / (_scale + dScale), 1.0f / (_scale + dScale), 1);
    glRotatef(-_angle - dAngle, 0, 0, 1);
    glTranslatef(-_x - dx, -_y - dy, 0);
}



















void CutsceneObject::RegisterFunction
    (int startTick, int endTick, std::string func, std::string params)
{
    // Use a switch to see which func should use
    void (CutsceneObject::*newFunc)(int, int, int, std::string);

    // Checks!!!                                                                            // This is where the cutscene obj functions are registered
    if (func == std::string("move"))
        newFunc = &CutsceneObject::Move;
    else if (func == std::string("move-lerp"))
        newFunc = &CutsceneObject::MoveLerp;
    else if (func == std::string("rotate"))
        newFunc = &CutsceneObject::Rotate;
    else if (func == std::string("scale"))
        newFunc = &CutsceneObject::Scale;
    else if (func == std::string("snap"))
        newFunc = &CutsceneObject::SetCoords;
    else if (func == std::string("wiggle-x"))
        newFunc = &CutsceneObject::WiggleX;
    else if (func == std::string("wiggle-y"))
        newFunc = &CutsceneObject::WiggleY;
    else if (func == std::string("change-sprite"))
        newFunc = &CutsceneObject::ChangeSprite;
    else if (func == std::string("fade-in"))
        newFunc = &CutsceneObject::FadeIn;
    else if (func == std::string("fade-out"))
        newFunc = &CutsceneObject::FadeOut;
    else if (func == std::string("delete-self"))
        newFunc = &CutsceneObject::DeleteMe;
    else if (func == std::string("exit-cutscene"))
        newFunc = &CutsceneObject::ExitCutscene;

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














/////////////////
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
        _x = (float)toX;
        _y = (float)toY;
        return;
    }

    // Find the distances
    float ratio_booboo =
        (float)(currentTick - startTick) / (float)(endTick - startTick);

    // Mix it into the delta vars!!!
    dx += (toX - _x) * ratio_booboo;
    dy += (toY - _y) * ratio_booboo;
}


void CutsceneObject::MoveLerp(int currentTick, int startTick, int endTick, std::string params)
{
    // Extract vars (ex: '0,0\t4')
    int toX, toY;
    float intensity;
    {
        std::string token;
        std::string subToken;
        std::istringstream tokenStream(params);

        std::getline(tokenStream, token, '\t');     // Check coords

        std::istringstream subTokenStream(token);   // Parse x and y from this!
        std::getline(subTokenStream, subToken, ',');
        std::istringstream(subToken) >> toX;
        std::getline(subTokenStream, subToken, ',');
        std::istringstream(subToken) >> toY;

        std::getline(tokenStream, token, '\t');     // Check intensity
        std::istringstream(token) >> intensity;
    }

    // See if last action... if so, set the x and y to this too!
    if (currentTick == endTick)
    {
        _x = (float)toX;
        _y = (float)toY;
        return;
    }

    // Find bubun
    int distance = endTick - startTick;
    float bubun = (currentTick - startTick) / (float)distance;

    // Plug into formula
    bubun = 1 - ((intensity + 1) * (1.0f / (bubun * intensity + 1)) - 1) / intensity;
    // Mix it into the delta vars!!!
    dx += (toX - _x) * bubun;
    dy += (toY - _y) * bubun;
}


void CutsceneObject::Rotate(int currentTick, int startTick, int endTick, std::string params)
{
    // Get the target angle (out of params) first of all
    float toAngle;
    {
        std::string token;
        std::istringstream tokenStream(params);

        std::getline(tokenStream, token);
        std::istringstream(token) >> toAngle;
    }



    // See if last action... if so, set the x and y to this too!
    if (currentTick == endTick)
    {
        _angle = toAngle;
        return;
    }

    // Find the distances
    float ratio_booboo =
        (float)(currentTick - startTick) / (float)(endTick - startTick);

    // Mix it into the delta vars!!!
    dAngle += (toAngle - _angle) * ratio_booboo;
}


void CutsceneObject::Scale(int currentTick, int startTick, int endTick, std::string params)
{
    // Get the target angle (out of params) first of all
    float toScale;
    {
        std::string token;
        std::istringstream tokenStream(params);

        std::getline(tokenStream, token);
        std::istringstream(token) >> toScale;
    }



    // See if last action... if so, set the x and y to this too!
    if (currentTick == endTick)
    {
        _scale = toScale;
        return;
    }

    // Find the distances
    float ratio_booboo =
        (float)(currentTick - startTick) / (float)(endTick - startTick);

    // Mix it into the delta vars!!!
    dScale += (toScale - _scale) * ratio_booboo;
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

    _x = (float)toX;
    _y = (float)toY;
}






float wiggle_helper(int currentTick, int startTick, int endTick, std::string params)        // This is a function!!!
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

    float angle = float(bubun * 2.0f * M_PI * backAndForth);
    return std::sin(angle) * variance;
}

void CutsceneObject::WiggleX(int currentTick, int startTick, int endTick, std::string params)
{
    dx += wiggle_helper(currentTick, startTick, endTick, params);
}
void CutsceneObject::WiggleY(int currentTick, int startTick, int endTick, std::string params)       // Just a copy of the x counterpart
{
    dy += wiggle_helper(currentTick, startTick, endTick, params);
}








void CutsceneObject::ChangeSprite(int currentTick, int startTick, int endTick, std::string params)
{
    // Get the sprite via the cutscene parent object!!!
    int spriteId;
    std::istringstream(params) >> spriteId;

    _image = cutscene->GetSpriteByID(spriteId);
}


void CutsceneObject::FadeIn(int currentTick, int startTick, int endTick, std::string params)
{
    if (currentTick == endTick)
    {
        sprAlpha = 1;
        return;
    }

    // Find alpha (from 0-1)
    int distance = endTick - startTick;
    float bubun = (currentTick - startTick) / (float)distance;

    // Apply the alpha
    sprAlpha = bubun;
}

void CutsceneObject::FadeOut(int currentTick, int startTick, int endTick, std::string params)
{
    if (currentTick == endTick)
    {
        sprAlpha = 0;
        return;
    }

    // Find alpha (from 1-0)
    int distance = endTick - startTick;
    float bubun = (currentTick - startTick) / (float)distance;

    // Apply the alpha
    sprAlpha = 1 - bubun;
}

void CutsceneObject::DeleteMe(int currentTick, int startTick, int endTick, std::string params)
{
    cutscene->Delete(this);
    delete this;
}





/// Params: name of the next thing to load! (i.e. 'c_%%%%' or 'n_%%%%')
void CutsceneObject::ExitCutscene(int currentTick, int startTick, int endTick, std::string params)
{
    std::istringstream tokenStream(params);

    std::string filename;
    std::getline(tokenStream, filename, '\t');

    int gx = -1, gy = -1;
    {
        std::string extraCoords;
        std::getline(tokenStream, extraCoords, '\t');

        if (!extraCoords.empty())
        {
            std::string coordPart;
            std::istringstream tokenStream(extraCoords);

            std::getline(tokenStream, coordPart, ',');
            std::istringstream(coordPart) >> gx;
            std::getline(tokenStream, coordPart, ',');
            std::istringstream(coordPart) >> gy;
        }
    }


    printf("Cutscene ended\n\n\n");

    Room* newRoom = NULL;

    std::string prefixLvl("n_");
    std::string prefixCut("c_");
    if (strncmp(filename.c_str(), prefixLvl.c_str(), prefixLvl.size()) == 0)
    {
        // The request is a level!!!!
        newRoom = new TestRoom(filename, cutscene->GetGameLoop(), gx, gy);
    }
    else if (strncmp(filename.c_str(), prefixCut.c_str(), prefixCut.size()) == 0)
    {
        // It's another cutscene!!!
        newRoom = new Cutscene(filename, cutscene->GetGameLoop());
    }

    // Error checking..
    if (newRoom == NULL)
    {
        printf("ERROR: Correct exiting prefix was not used!!!\n");
        return;
    }

    // Add the created room to here!
    cutscene->End(newRoom);
}
