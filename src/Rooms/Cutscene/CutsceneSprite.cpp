#include "CutsceneSprite.h"

CutsceneSprite::CutsceneSprite(Quad* firstImg)
{
    AddImage(firstImg);
    ticksPerFrame = 1;
    repeat = true;
}

CutsceneSprite::~CutsceneSprite()
{
    for (int i = 0; i < images.size(); i++)
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







void CutsceneSprite::Render(int x, int y, int ticks)
{
    if (images.size() == 1)
    {
        images.at(0)->Render(x, y);
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
        currentFrame = ticks < images.size() ? ticks : images.size() - 1;
    }

    images.at(currentFrame)->Render(x, y);
}

