#include "Textbox.h"
#include <algorithm>
#include <sstream>

Textbox::Textbox(std::string text, int fontSize, TestRoom* rm) : Object(0, 0, rm, false)
{
    _fontSize = fontSize;

    // Get the text divied up!
    lines = std::count(text.begin(), text.end(), '\n') + 1;

    _textLines = new std::string[lines];
    _renderingText = new Quad*[lines];
    std::string token;
    std::istringstream tokenStream(text);
    for (int i = 0; i < lines; i++)
    {
        // Grab each individual line!
        std::getline(tokenStream, token, '\n');
        _textLines[i] = token;

        // Create quads for each line
        Texture* tempTex = new Texture(_textLines[i], ".data/fonts/CATHSGBR.TTF", _fontSize);
        _renderingText[i] = new Quad(tempTex->GetWidth(), tempTex->GetHeight(), tempTex);
    }

    //
}

Textbox::~Textbox()
{
    //dtor
}

void Textbox::Update()
{
    // Increase ticks and move thru the text!
}

void Textbox::Render()
{
    for (int i = 0; i < lines; i++)
    {
        _renderingText[i]->Render(1400, 1500 + _fontSize * i);
    }
}
