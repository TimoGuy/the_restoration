#pragma once

#ifdef __unix__
#include "Object.h"
#include "Textbox.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../Object.h"
#include "Textbox.h"
#endif

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
