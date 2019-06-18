#include "Textbox.h"
#include <algorithm>
#include <sstream>

Textbox::Textbox(float x, float y, std::string text, int fontSize, TestRoom* rm) : Object(0, 0, rm, false)
{
    // Directly set the variables
    SetCoords(x, y);
    _fontSize = fontSize;

    // Open the font
    TTF_Font *font = TTF_OpenFont(".data/fonts/CATHSGBR.TTF", _fontSize);
    if (font == nullptr)
    {
        printf("Font-texture could not initialize! TTF_Error: %s\n", TTF_GetError());
        return;
    }

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
        Texture* tempTex = new Texture(_textLines[i], font);
        _renderingText[i] = new Quad(tempTex->GetWidth(), tempTex->GetHeight(), tempTex);
    }

    // Clean up!
    TTF_CloseFont(font);
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
        _renderingText[i]->Render(x, y + _fontSize * i);
    }
}


void Textbox::SetCoords(float x, float y)
{
    this->x = x;
    this->y = y;
}
