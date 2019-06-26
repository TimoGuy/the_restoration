#pragma once
#ifdef __unix__
#include "Quad.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../../Shape/Quad.h"
#endif

#include <vector>

class CutsceneSprite
{
    public:
        CutsceneSprite(Quad* firstImg);
        virtual ~CutsceneSprite();

        void AddImage(Quad* nextImg);

        void SetTicksPerFrame(int tpf);
        void SetRepeat(bool flag);

        void Render(int x, int y, int ticks, float alpha);

    protected:

    private:
        std::vector<Quad*> images;
        int ticksPerFrame;
        bool repeat;
};
