#pragma once

#ifdef __unix__
#include "Object.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../Object.h"
#endif

class TestRoom;


#include <string>
#include <vector>
#include <functional>


class Textbox : public Object
{
    public:
        Textbox(float x, float y, std::string fileName, int fontSize, const std::function<void()>& lambda, TestRoom* rm);
        Textbox(float x, float y, std::vector<std::string> text, int fontSize, const std::function<void()>& lambda, TestRoom* rm);
        virtual ~Textbox();

        void Update();
        void Render();

        void SetXY(float x, float y);

        bool IsColliding(BoundBox* box) { return false; }

        bool DeleteMe();

    protected:

    private:
        void SetupTextbox(float x, float y, std::string text);                      bool setupReq = true;

		bool skipping = false;		// This is only for pressing button3 to skip the whole textbox!!!

        void OnExitRequest();                                                       bool exiting = false;
        std::function<void()> _lambda;


        int ticks = 0;
        bool done = false;

        float masterAlphaOff = 0;
        float masterYoff = 0;
        float originalHeight = 0;

        int exitTicks = -1;     // Will check if less than 0 first!

        // For continuation of textboxes
        int currentString;
        std::vector<std::string> _strings;

        // For current textbox
        int _fontSize;

        std::vector<std::string> _textLines;
        std::vector<Quad*> _renderingText;
        std::vector<float> _textAlpha;

        Quad* background;
};
