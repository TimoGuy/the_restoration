#pragma once

#if defined(__unix__) || defined(__APPLE__)
#include "Object.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../Object.h"
#endif

struct BoundBox;
class Textbox;
class Quad;


class Npc : public Object
{
    public:
        Npc(int gx, int gy, TestRoom* rm);
        virtual ~Npc();

        void Update();
        void Render();

		bool IsColliding(BoundBox* box) { return false; }

    protected:
    	Quad* image;

    private:
        Textbox* tBox;

        void OnTextboxFinish();
};
