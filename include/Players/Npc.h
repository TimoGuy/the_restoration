#pragma once

#include "Object.h"
#include "Textbox.h"

class Npc : public Object
{
    public:
        Npc(int gx, int gy, TestRoom* rm);
        virtual ~Npc();

        void Update();
        void Render();

        bool IsColliding(BoundBox* box) { }

    protected:
    	Quad* image;

    private:
        Textbox* tBox;

        void OnTextboxFinish();
};
