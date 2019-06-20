#pragma once

#include "Object.h"
#include "TestRoom.h"

#include <string>
#include <vector>
#include <functional>


class Textbox : public Object
{
    public:
        Textbox(float x, float y, std::string text, int fontSize, const std::function<void()>& lambda, TestRoom* rm);
        virtual ~Textbox();

        void Update();
        void Render();

        void SetXY(float x, float y);

        bool IsColliding(BoundBox* box) { return false; }

        void SetCoords(float x, float y);

    protected:

    private:
        void OnExitRequest();       bool execOnExit = false;
        std::function<void()> _lambda;


        int ticks = 0;

        float masterAlphaOff = 0;
        float masterYoff = 0;

        int exitTicks = -1;     // Will check if less than 0 first!


        int _fontSize;
        std::string _rawText;

        std::vector<std::string> _textLines;
        std::vector<Quad*> _renderingText;
        std::vector<float> _textAlpha;

        Quad* background;
};
