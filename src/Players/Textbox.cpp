#if defined(__unix__) || defined(__APPLE__)
#include "Textbox.h"
#include "InputManager.h"
#include <SDL2/SDL_ttf.h>
#include "Lib/Texture.h"
#include "Quad.h"
#include "TestRoom.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Players/Textbox.h"
#include "../../include/InputManager.h"
#include <SDL_ttf.h>
#include "../../include/Lib/Texture.h"
#include "../../include/Shape/Quad.h"
#include "../../include/Rooms/TestRoom.h"
#endif

#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>




// Variables eh
//#define FONT_NAME ".data/fonts/CATHSGBR.TTF"
//#define FONT_NAME ".data/fonts/Sword.ttf"
#define FONT_NAME ".data/fonts/851MkPOP_002.ttf"


    // SPEED_DAMPER ---- slow breathing->300, worried->70||75, frightened->10
#define SPEED_DAMPER 300.0f
    // WAVE_AMPLITUDE ---- slow breathing->2.5, worried->1.25, frightened->0.5
#define WAVE_AMPLITUDE 2.5f
#define PADDING 10

#define EVERY_X_TICKS_NEW_WORD 10
#define ALPHA_INCREASE 0.02f

#define ENTER_TICKS 25
#define ENTER_MAX_YOFF 25
#define EXIT_TICKS 25
#define EXIT_MAX_YOFF 50



std::string theFont = FONT_NAME;

Textbox::Textbox(float x, float y, std::string fileName, int fontSize, const std::function<void()>& lambda, TestRoom* rm) : Object(0, 0, rm, false)
{
    // Load text or string
    std::vector<std::string> texts;

    std::string path = std::string(".data/fonts/") + fileName;

    // Load all fonts and such!
    std::string line;
    std::ifstream myfile(path + ".txt");

    std::string buildingString;
    if (myfile.is_open())
    {
        while (std::getline(myfile, line))
        {
            // Load the line
            if (line.at(0) == 'q')
            {
                // This means quit the reading!
                break;
            }
            else if (line.at(0) == 'f')
            {
                // Request the font!!! ('f' then '\t', so cut off at 2)
                theFont = line.substr(2);
            }
            else if (line.at(0) == 't')
            {
                // Should be 't' then '\t' then the text
                if (!buildingString.empty())
                {
                    // New string! so put old string into the list
                    texts.push_back(buildingString);
                }

                // Reset
                buildingString.clear();
                buildingString += line.substr(2);       // Remove 't' and '\t', so 2!
            }
            else if (line.at(0) == '\t')
            {
                // Append to building string.
                buildingString += "\n" + line.substr(1);       // Remove the '\t'
            }
            else
            {
                // Just stuff the line in I guess....
                printf("WARNING: Use the 't\t' system to create text dumps!!!\n");
                texts.push_back(line);
            }
        }
    }

    // Final stuff eh.
    texts.push_back(buildingString);

    // Setup!
    _strings = texts;
    _fontSize = fontSize;
    _lambda = lambda;

    // Setup the first textbox!!!!
    currentString = 0;
    SetupTextbox(x, y, _strings.at(currentString));
}


Textbox::Textbox(float x, float y, std::vector<std::string> strings, int fontSize, const std::function<void()>& lambda, TestRoom* rm) : Object(0, 0, rm, false)
{
    // Setup!
    _strings = strings;
    _fontSize = fontSize;
    _lambda = lambda;

    // Setup the first textbox!!!!
    currentString = 0;
    SetupTextbox(x, y, _strings.at(currentString));
}

Textbox::~Textbox()
{
    //dtor
}


void Textbox::SetupTextbox(float x, float y, std::string text)
{
    // Init start values
    ticks = 0;
    masterAlphaOff = 0;
    masterYoff = 0;
    exitTicks = -1;
    exiting = false;

    for (unsigned int i = 0; i < _renderingText.size(); i++)
    {
        delete _renderingText.at(i);
    }
    _textLines.clear();
    _renderingText.clear();
    _textAlpha.clear();



    // Open the font
    TTF_Font* font = TTF_OpenFont(theFont.c_str(), _fontSize);
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
            // See if there's any delay markers '`' IN THE BEGINNING!!!
            while (!subToken.empty() &&
                   subToken.at(0) == '`')
            {
                // Remove the delay marker
                subToken = subToken.substr(1);

                // Add a dead entry.
                _textLines.push_back("");
                _renderingText.push_back(NULL);
                _textAlpha.push_back(-1);
            }

            _textLines.push_back(subToken + " ");       // Add a space, el paco!

            // Create quads for each line
            Texture* tempTex = new Texture(subToken + " ", font);
            _renderingText.push_back(new Quad((float)tempTex->GetWidth(), (float)tempTex->GetHeight(), tempTex));
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
    height = (float)_fontSize * lines;      // Calc the height (width is already calc'd)

    // Add the padding
    width += PADDING * 2;
    height += PADDING * 2;


    background = new Quad(width, height);

    // Save original height
    originalHeight = height;

    // Finally, set coords!
    SetXY(x, y);
}






void Textbox::Update()
{
    // Setup the thing if wanted!!!
    if (setupReq)
    {
        setupReq = false;
        SetupTextbox(x, y, _strings.at(currentString));
    }

    // Increase ticks and move thru the text!
    ticks++;

    // Check if exit textbox was pressed!
    if (InputManager::Instance().b1() &&
        !exiting)        // <--- This makes it so that you can't 'resurrect' a dying textbox.h!!!
    {
        exitTicks = ticks;
    }

	// Or if the player just wants to skip it outright!
	else if (InputManager::Instance().b3() &&
		!skipping)
	{
		skipping = true;

		// Do the screen transition
		auto skipLambda= [&](void)
		{
			OnExitRequest();	// This means I quit, but once I'm hidden this will execute
			// NOTE: this will also run this textbox's lambda too!

			room->ScreenFadeIn();
		};
		room->ScreenTransition(skipLambda);
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
        exiting = true;

        // Yeah!!! Now it's the reverse!
        masterAlphaOff = (float)(ticks - exitTicks) / EXIT_TICKS * -1.5f;   // Starts at zero and goes down to -1.5 eh!
        masterYoff = -EXIT_MAX_YOFF * (1.0f - (float)(EXIT_TICKS - (ticks - exitTicks)) / EXIT_TICKS);
    }
    else if (exitTicks > ENTER_TICKS &&
             ticks - exitTicks > EXIT_TICKS)
    {
        // You're invisible, let's re-init or delete!!!!
        currentString++;
        if (currentString < (signed int)_strings.size())
        {
            setupReq = true;
        }
        else
        {
            // You're quitting now...
            OnExitRequest();
        }
    }


    // Render the bg

    glColor4f(0, 0, 0, 0.5f + masterAlphaOff);
    background->Render(x, y + masterYoff);

    background->SetHeight(originalHeight);






    // RENDER TEXT!!!!
    float addToHeight = 0;

    int offset = 0;
    int line = 0;
    for (int i = 0; i < (signed int)_textLines.size() && i < ticks / EVERY_X_TICKS_NEW_WORD; i++)
    {
        if (_textLines.at(i) == "\n")
        {
            // Newline!!!
            offset = 0;
            line++;
        }
        else if (_textLines.at(i).empty())
        {
            // Just ignore this one!
            continue;
        }
        else
        {
            float rendX = x + PADDING + offset;
			float rendY = y + PADDING + _fontSize * line;



            // Wiggle!
            float bubun = ticks / SPEED_DAMPER;

            float angle = float(bubun * 2.0f * M_PI);
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
            offset += (int)_renderingText.at(i)->GetWidth();         // Offset the width (includes space!)


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
    // Run that function!!!
    if (_lambda)
        _lambda();
    else
        printf("ERROR: no lambda was stored in message box!\n");

    // Die
    done = true;
}
