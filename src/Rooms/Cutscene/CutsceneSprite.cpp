#ifdef __unix__
#include "CutsceneSprite.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../../../include/Rooms/Cutscene/CutsceneSprite.h"
#endif

CutsceneSprite::CutsceneSprite(Quad* firstImg)
{
    AddImage(firstImg);
    ticksPerFrame = 1;
    repeat = true;
}

CutsceneSprite::~CutsceneSprite()
{
    for (unsigned int i = 0; i < images.size(); i++)
    {
        delete images.at(i);
    }
    images.clear();
}





void CutsceneSprite::AddImage(Quad* nextImg)
{
    images.push_back(nextImg);
}

void CutsceneSprite::SetTicksPerFrame(int tpf) { ticksPerFrame = tpf; }
void CutsceneSprite::SetRepeat(bool flag) { repeat = flag; }






void CutsceneSprite::Render(int x, int y, int ticks, float alpha)
{
    // Set the color!
    glColor4f(1, 1, 1, alpha);

    if (images.size() == 1)
    {
        images.at(0)->Render((float)x, (float)y);
        return;
    }




    // If there's 2 ticks per frame
    // then it will just switch 2x slower.
    ticks /= ticksPerFrame;

    int currentFrame = 0;
    if (repeat)
    {
        // Use this for current frame
        currentFrame = ticks % images.size();
    }
    else
    {
        currentFrame = ticks < (signed int)images.size() ? ticks : (signed int)images.size() - 1;
    }

    images.at(currentFrame)->Render((float)x, (float)y);


    // Reset color
    glColor4f(1, 1, 1, 1);
}

