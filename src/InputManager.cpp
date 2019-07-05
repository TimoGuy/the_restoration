#ifdef __unix__
#include "InputManager.h"
#include "SDL2/SDL.h"
#include <SDL2/SDL_opengl.h>
#include "defs.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../include/InputManager.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include "../include/defs.h"
#endif
#include <stdio.h>
#include <cmath>
#include <algorithm>



bool InputManager::reloadResource()
{
	if (_reloadRes)
	{
		_reloadRes = false;
		return true;
	}
	return _reloadRes;
}







InputManager::InputManager()
{
    // Try initializing 1 joystick (gamecontroller)!
	if ((numJoysticks = SDL_NumJoysticks()) < 1)
	{
		printf("No joysticks!!\n");
	}
	else
	{
		for (int i = 0; i < numJoysticks; i++)
		{
			if (SDL_IsGameController(i))
			{
				// Get first compatible one!
				controller = SDL_GameControllerOpen(i);
				break;
			}
		}

		if (controller == NULL)
		{
			printf("No compatible game controllers!!\n");
			return;
		}
		else
		{
			printf("Controller Name: %s\n", SDL_GameControllerName(controller));
		}
	}
}

InputManager::~InputManager()
{
    //dtor
	SDL_GameControllerClose(controller);
}

void InputManager::ProcessInput(GameLoop* g)
{
    // Poll those events!!!!
    SDL_Event e;


    while(SDL_PollEvent(&e) != 0)
    {
        // Click close button
        if(e.type == SDL_QUIT)
        {
            // Exit the game loop
            g->TerminateLoop();
        }
        else if (e.type == SDL_WINDOWEVENT)
        {
            // Window events
            switch (e.window.event)
            {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    g->SetWindowDimensions(e.window.data1, e.window.data2);
                    break;
            }
        }
        else
        {
            // Handle the keyboard

            // ESCAPE TO QUIT
            if (e.type == SDL_KEYUP &&
                e.key.keysym.sym == SDLK_ESCAPE)
            {
                g->TerminateLoop();
            }



            // Handle "controller" events

            // KEYBOARD INPUT CHECK... See project's ReadMe!!!!!
            if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_w:
                    kUp = true;
                    break;

                case SDLK_s:
                    kDown = true;
                    break;

                case SDLK_a:
                    kLeft = true;
                    break;

                case SDLK_d:
                    kRight = true;
                    break;



                case SDLK_j:
                    _b1 = true;
                    break;

                case SDLK_k:
                    _b2 = true;
                    break;

				case SDLK_SPACE:
					_b3 = true;
					break;

				case SDLK_r:
					_reloadRes = true;		// FOR DEBUG: there's no keyup event, btw.
					break;

				case SDLK_LCTRL:
				case SDLK_RCTRL:
					_b4 = !_b4;				// FOR DEBUG: a toggling btn to show the original blox
					printf("InputManager:: Toggled showing collision blocks!\n");
					break;
                }
            }
            else if (e.type == SDL_KEYUP)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_w:
                    kUp = false;
                    break;

                case SDLK_s:
                    kDown = false;
                    break;

                case SDLK_a:
                    kLeft = false;
                    break;

                case SDLK_d:
                    kRight = false;
                    break;



                case SDLK_j:
                    _b1 = false;
                    break;

                case SDLK_k:
                    _b2 = false;
                    break;

				case SDLK_SPACE:
					_b3 = false;
					break;
                }
            }



			// HANDLE THE GAMECONTROLLER
#define JOYSTICK_MAX_RADIUS 32767.0f
#define JOYSTICK_RAD_DEADZONE 0.18f
			else if (e.type == SDL_CONTROLLERAXISMOTION)
			{
				if (e.caxis.which ==
					SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller)))		// First controller
				{
					if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)		// X axis
					{
						_joy_x = std::min(1.0f, std::max(-1.0f, e.caxis.value / JOYSTICK_MAX_RADIUS));
						if (std::abs(_joy_x) < JOYSTICK_RAD_DEADZONE)
						{
							// DEADZONE it!
							_joy_x = 0;
						}
					}
					else if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)		// Y axis
					{
						_joy_y = std::min(1.0f, std::max(-1.0f, e.caxis.value / JOYSTICK_MAX_RADIUS));
						if (std::abs(_joy_y) < JOYSTICK_RAD_DEADZONE)
						{
							// DEADZONE it!
							_joy_y = 0;
						}
					}
				}

			}
			else if (e.type == SDL_CONTROLLERBUTTONDOWN)
			{
				// Press buttons!
				if (e.cbutton.button == SDL_CONTROLLER_BUTTON_A)
					_b2 = true;
				else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_X)
					_b1 = true;
				else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_B)
					_b3 = true;
			}
			else if (e.type == SDL_CONTROLLERBUTTONUP)
			{
				// Release
				if (e.cbutton.button == SDL_CONTROLLER_BUTTON_A)
					_b2 = false;
				else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_X)
					_b1 = false;
				else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_B)
					_b3 = false;
			}
        }
    }


	// The keyboard is the big boss when the controller's not being used!
    if (_joy_x != 0 || _joy_y != 0)
    {
        _x = _joy_x;
        _y = _joy_y;
    }

    // Okay then, now poll the keyboard, since the gc's not being used!!!
    else
	{
		// ESPECIALLY FOR KEYBOARD... since the variables need some refining eh.
		if (kLeft == kRight) _x = 0;   // If both are pressed it cancels
		else if (kLeft && !kRight) _x = -1.0f;
		else if (!kLeft && kRight) _x = 1.0f;

		if (kUp == kDown) _y = 0;
		else if (kUp && !kDown) _y = -1.0f;
		else if (!kUp && kDown) _y = 1.0f;
	}
}
