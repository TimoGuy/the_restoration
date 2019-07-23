#ifdef __unix__
#include "GameLoop.h"
#include "Rooms/TestRoom.h"
#include "Cutscene.h"
#include <SDL2/SDL_opengl.h>
#include "InputManager.h"
#include "SerialManager.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../include/GameLoop.h"
#include "../include/Rooms/TestRoom.h"
#include "../include/Rooms/Cutscene/Cutscene.h"
#include <SDL_opengl.h>
#include "../include/InputManager.h"
#include "../include/SerialManager.h"
#endif


#include <iostream>

std::string prevInputCode;




GameLoop::GameLoop(SDL_Window* window)
{
    LoadGameVariables();
    _window = window;

    // Set the initial shape for the window
	SetWindowDimensions(SCREEN_WIDTH, SCREEN_HEIGHT);
}

bool GameLoop::Execute()
{
    // This will be the Room obj
    SetRoom(new Cutscene("c_first", this));     // This is the beginning thing of the game eh!

    // This will hold up the thread...
    // I'll be honest, there's really only
    // supposted to be 1 main thread you know?
    // TODO: maybe make this a singleton or something????

	int msAllotted = 1000 / 60;		// Unfortunately there's no such thing as 'half a millisecond', so it needs to be an int
	unsigned int targetTime = SDL_GetTicks();

    while (_running)
    {
		// Reset messages!
		_didJustResize = false;


        // Event system
        InputManager::Instance().ProcessInput(this);    // If the input desires, 'this' game loop can be terminated from this function!

		// Update
        GetRoom()->Update();



		// Render
		glClear(GL_COLOR_BUFFER_BIT);

        GetRoom()->Render();



        SDL_GL_SwapWindow(_window);






        // FOR DEBUG: check if the room needs to be reloaded
        if (InputManager::Instance().reloadResource())		// This is a check, but after 1 check the inside variable turns off, so no worries.
        {
            // EDIT: Make it so that you can change the resource to test using <iostream>
            std::cout << "Please enter the new resource\'s name\n(Press enter to reload \"" << prevInputCode.c_str() << "\"): ";
            std::string newRes;
            std::getline(std::cin, newRes);
            if (newRes.empty())
            {
                // Try loading the previous one!
                if (prevInputCode.empty())
                    printf("ERROR: Resource name was empty!!\n");
                else
                {
                    // Prep for loading the resource
                    newRes = prevInputCode;
                }
            }


            // So, did the new resource actually get filled?
            if (!newRes.empty())
            {
                // Load the filled data!!!
                switch (newRes.at(0))
                {
                    case 'n':
                        // Load the level eh
                        SetRoom(new TestRoom(newRes.c_str(), this));
                        break;

                    case 'c':
                        // Cutscene time!!!
                        SetRoom(new Cutscene(newRes.c_str(), this));
                        break;
                }

                // For the next time!!!
                prevInputCode = newRes;
            }
        }







        // See if any wait frames
        if (waitTicks > 0)
        {
            // Do the delay
            SDL_Delay(msAllotted * waitTicks);
            targetTime += msAllotted * waitTicks;

            // Reset eh
            waitTicks = 0;
        }


		// Matouze
        if (SDL_GetTicks() >= (targetTime += msAllotted))
        {
            // Either right on or deficit (lagging)
            printf("This frame lagged!\n");
            targetTime = SDL_GetTicks();    // Get back on track!
        }
        else
        {
            // Delay for next frame
            SDL_Delay((unsigned int)targetTime - SDL_GetTicks());
        }

		//SDL_Delay(1000 / 30);		// Clamp to <30 fps for debugging
    }

    return true;
}



void GameLoop::SetRoom(Room* newRoom)
{
    // Remove current one
//    if (currentRoom != NULL)              // TODO:: there's problems w/ delete currentRoom!!!!!!
//        delete currentRoom;

    currentRoom = newRoom;        // This will shift to the currentRoom when the time is right ;)
}
Room* GameLoop::GetRoom() { return currentRoom; }


GameLoop::~GameLoop()
{
    // Thanks for letting me use it!
    _window = NULL;
}






void GameLoop::AddPause(int numTicks)
{
    waitTicks += numTicks;
    // This will be handled in the
    // main game loop right before
    // the whole big pause for the next frame!
}







void GameLoop::SetWindowDimensions(int width, int height)
{
	// Enable the message!!!
	_didJustResize = true;




    // Make sure to have the new dimensions recorded!!!
    _windowWidth = width;
    _windowHeight = height;

    // Set the viewport
    glViewport(0, 0, width, height);

    // Initialize Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width / 2, width / 2, height / 2, -height / 2, 1.0, -1.0);

    // Initialize Modelview Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GameLoop::GetWindowDimensions(int& width, int& height)
{
    width = _windowWidth;
    height = _windowHeight;
}

bool GameLoop::DidJustResize()
{
	return _didJustResize;
}

void GameLoop::LoadGameVariables()
{
    SerialManager::Instance().GetGameData_Bool("saw_tutorial", GAME_VAR_DEF_saw_tutorial);
    SerialManager::Instance().GetGameData_Int("player_max_jumps", GAME_VAR_DEF_player_max_jumps);
}
