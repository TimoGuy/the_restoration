#include "Textbox.h"
#include <algorithm>
#include <sstream>




// Variables eh
#define SPEED_DAMPER 300.0f
#define WAVE_AMPLITUDE 2.5f
#define PADDING 10

float originalHeight = 0;
Textbox::Textbox(float x, float y, std::string text, int fontSize, TestRoom* rm) : Object(0, 0, rm, false)
{
    // Set coords
    this->x = x;
    this->y = y;
    _fontSize = fontSize;

    // Open the font
    TTF_Font *font = TTF_OpenFont(".data/fonts/CATHSGBR.TTF", _fontSize);
    if (font == nullptr)
    {
        printf("Font-texture could not initialize! TTF_Error: %s\n", TTF_GetError());
        return;
    }

    // Get the text divied up!
    std::string token;
    std::istringstream tokenStream(text);

    float width = 0, height = 0;
    int lines = 0;

    // Grab each individual line!
    while (std::getline(tokenStream, token, '\n'))
    {
        lines++;
        std::string subToken;
        std::istringstream subTokenStream(token);
        // So,,,, further subdivide the token eh!

        float tempWidth = 0;        // See who's biggest!!
        while (std::getline(subTokenStream, subToken, ' '))
        {
            _textLines.push_back(subToken + " ");       // Add a space, el paco!

            // Create quads for each line
            Texture* tempTex = new Texture(subToken + " ", ".data/fonts/CATHSGBR.TTF", _fontSize);
            _renderingText.push_back(new Quad(tempTex->GetWidth(), tempTex->GetHeight(), tempTex));

            tempWidth += tempTex->GetWidth();
        }

        // Get the BBD!
        width = std::max(width, tempWidth);

        // Add delimiter for rendering.
        _textLines.push_back("\n");
        _renderingText.push_back(NULL);
    }





    // Create the actual textbox bg.
    height = fontSize * lines;      // Calc the height (width is already calc'd)

    // Add the padding
//    x -= PADDING;
    width += PADDING * 2;
//    y -= PADDING;
    height += PADDING * 2;


    background = new Quad(width, height);

    // Save original height
    originalHeight = height;
}

Textbox::~Textbox()
{
    //dtor
}

void Textbox::Update()
{
    // Increase ticks and move thru the text!
    ticks++;
}


void Textbox::Render()
{
    // Render the bg

    glColor4f(0, 0, 0, 0.5f);
    background->Render(x, y);

    background->SetHeight(originalHeight);


    // RENDER TEXT!!!!
    glColor4f(1, 1, 1, 1);

    float addToHeight = 0;

    int offset = 0;
    int line = 0;
    for (int i = 0; i < _textLines.size() && i < ticks / 5; i++)
    {
        if (_textLines.at(i) == "\n")
        {
            // Newline!!!
            offset = 0;
            line++;
        }
        else
        {
            int rendX = x + PADDING + offset;
            int rendY = y + PADDING + _fontSize * line;



            // Wiggle!
            float bubun = ticks / SPEED_DAMPER;

            float angle = bubun * 2.0f * M_PI;
            float addition = (-std::cos(angle) + 1) * WAVE_AMPLITUDE * (line + 1);
            rendY += addition;
            addToHeight = addition;         // We just want the last one, which will be the largest












            // Actually render eh.
            _renderingText.at(i)->Render(rendX, rendY);
            offset += _renderingText.at(i)->GetWidth();         // Offset the width (includes space!)


        }
    }


    // Add to height (before it renders, okay???)
    background->SetHeight(background->GetHeight() + addToHeight);
}


void Textbox::SetXY(float x, float y)
{
    this->x = x;
    this->y = y;
}
