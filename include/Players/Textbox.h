#pragma once

#include "Object.h"
#include "TestRoom.h"

#include <string>


class Textbox : public Object
{
    public:
        Textbox(float x, float y, std::string text, int fontSize, TestRoom* rm);
        virtual ~Textbox();

        void Update();
        void Render();

        bool IsColliding(BoundBox* box) { return false; }

        void SetCoords(float x, float y);

    protected:

    private:
        int _fontSize;
        std::string _rawText;

        int lines;
        std::string* _textLines;
        Quad** _renderingText;
};
