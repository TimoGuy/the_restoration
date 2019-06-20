#include "Textbox.h"
#include "InputManager.h"
#include <algorithm>
#include <sstream>




// Variables eh
    // SPEED_DAMPER ---- slow breathing->300, worried->70||75, frightened->10
#define SPEED_DAMPER 300.0f
    // WAVE_AMPLITUDE ---- slow breathing->2.5, worried->1.25, frightened->0.5
#define WAVE_AMPLITUDE 2.5f
#define PADDING 10

#define EVERY_X_TICKS_NEW_WORD 20
#define ALPHA_INCREASE 0.02f

#define ENTER_TICKS 50
#define ENTER_MAX_YOFF 25
#define EXIT_TICKS 50
#define EXIT_MAX_YOFF 50



Textbox::Textbox(float x, float y, std::string text, int fontSize, const std::function<void()>& lambda, TestRoom* rm) : Object(0, 0, rm, false)
{
    // Setup!
    _fontSize = fontSize;
    _lambda = lambda;

    // Open the font
    TTF_Font* font = TTF_OpenFont(".data/fonts/CATHSGBR.TTF", _fontSize);
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



    // Grab each individual line! (Uses the same font over and over eh!)
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
            Texture* tempTex = new Texture(subToken + " ", font);
            _renderingText.push_back(new Quad(tempTex->GetWidth(), tempTex->GetHeight(), tempTex));
            _textAlpha.push_back(0);    // Set at transparent so that it can fade in eh.

            tempWidth += tempTex->GetWidth();
        }

        // Get the BBD!
        width = std::max(width, tempWidth);

        // Add delimiter for rendering.
        _textLines.push_back("\n");
        _renderingText.push_back(NULL);
        _textAlpha.push_back(-1);
    }


    // Clean up.
    TTF_CloseFont(font);




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

    // Finally, set coords!
    SetXY(x, y);
}

Textbox::~Textbox()
{
    //dtor
}

void Textbox::Update()
{
    // Increase ticks and move thru the text!
    ticks++;

    // Check if exit textbox was pressed!
    if (InputManager::Instance().b1() &&
        !execOnExit)        // <--- This makes it so that you can't 'resurrect' a dying textbox.h!!!
    {
        exitTicks = ticks;
    }
}


void Textbox::Render()
{
    // Enter in the textbox!!!
    if (ticks <= ENTER_TICKS)
    {
        // Fade and shift in!
        masterAlphaOff = (float)ticks / ENTER_TICKS - 1.0f;
        masterYoff = ENTER_MAX_YOFF * (float)(ENTER_TICKS - ticks) / ENTER_TICKS;
    }
    else if (exitTicks > ENTER_TICKS &&     // The exit ticks need to be after it's all done loading, hence the ENTER_TICKS
             ticks - exitTicks <= EXIT_TICKS &&
             ticks != exitTicks)        // <---- This makes it onKeyRelease eh!
    {
        // You're exiting now.. (this func. only runs once then disallows another run)
        OnExitRequest();

        // Yeah!!! Now it's the reverse!
        masterAlphaOff = (float)(ticks - exitTicks) / EXIT_TICKS * -1.5f;   // Starts at zero and goes down to -1.5 eh!
        masterYoff = -EXIT_MAX_YOFF * (1.0f - (float)(EXIT_TICKS - (ticks - exitTicks)) / EXIT_TICKS);
    }
    else if (exitTicks > ENTER_TICKS &&
             ticks - exitTicks > EXIT_TICKS)
    {
        // You're invisible, let's delete yah!
        done = true;        // <---- The garbage collecting variable eh.
    }


    // Render the bg

    glColor4f(0, 0, 0, 0.5f + masterAlphaOff);
    background->Render(x, y + masterYoff);

    background->SetHeight(originalHeight);






    // RENDER TEXT!!!!
    float addToHeight = 0;

    int offset = 0;
    int line = 0;
    for (int i = 0; i < _textLines.size() && i < ticks / EVERY_X_TICKS_NEW_WORD; i++)
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







            // Set the alpha
            float* alpha = &_textAlpha.at(i);
            *alpha += ALPHA_INCREASE;            // Increase until 1 eh
            *alpha = std::min(*alpha, 1.0f);
            glColor4f(1, 1, 1, *alpha + masterAlphaOff);




            // Actually render eh.
            _renderingText.at(i)->Render(rendX, rendY + masterYoff);
            offset += _renderingText.at(i)->GetWidth();         // Offset the width (includes space!)


        }
    }


    // Add to height (before it renders, okay???)
    background->SetHeight(background->GetHeight() + addToHeight);
}


void Textbox::SetXY(float x, float y)
{
    // Finally, adjust coords so that textbox is centered
    this->x = x - (background->GetWidth() / 2);
    this->y = y - (originalHeight / 2);
}

bool Textbox::DeleteMe()
{
    return done;
}




void Textbox::OnExitRequest()
{
    if (execOnExit)
    {
        // Already run once eh.
        return;
    }

    // Make it so can't be run again eh.
    execOnExit = true;

    // Run that function!!!
    if (_lambda)
        _lambda();
    else
        printf("ERROR: no lambda was stored in message box!\n");
}
