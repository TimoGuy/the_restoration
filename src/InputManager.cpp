#include "InputManager.h"
#include "SDL2/SDL.h"

InputManager::InputManager()
{
    //ctor
}

InputManager::~InputManager()
{
    //dtor
}

void InputManager::ProcessInput(GameLoop* g)
{
    // Poll those events!!!!
    SDL_Event e;
    // TODO: Figure out some joystick overrides keyboard system!


    while(SDL_PollEvent(&e) != 0)
    {
        // Click close button
        if(e.type == SDL_QUIT)
        {
            // Exit the game loop
            g->TerminateLoop();
        }
        // else if (e.type == SDL_WINDOWEVENT)
        // {
        //     // Window events
        //     switch (e.window.event)
        //     {
        //         case SDL_WINDOWEVENT_SIZE_CHANGED:
        //             Reshape(e.window.data1, e.window.data2);
        //             break;
        //     }
        // }
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
                }
            }


        }
    }


    // ESPECIALLY FOR KEYBOARD... since the variables need some refining eh.
    if (kLeft == kRight) _x = 0;   // If both are pressed it cancels
    else if (kLeft && !kRight) _x = -1.0f;
    else if(!kLeft && kRight) _x = 1.0f;

    if (kUp == kDown) _y = 0;
    else if (kUp && !kDown) _y = -1.0f;
    else if(!kUp && kDown) _y = 1.0f;
}
