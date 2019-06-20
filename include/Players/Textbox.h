#pragma once

#include "Object.h"
#include "TestRoom.h"

#include <string>
#include <vector>


class Textbox : public Object
{
    public:
        Textbox(int x, int y, std::string text, int fontSize, TestRoom* rm);
        virtual ~Textbox();

        void Update();
        void Render();

        void SetXY(float x, float y);

        bool IsColliding(BoundBox* box) { return false; }

    protected:

    private:
        int ticks = 0;


        int _fontSize;
        std::string _rawText;

        std::vector<std::string> _textLines;
        std::vector<Quad*> _renderingText;

        Quad* background;
};
