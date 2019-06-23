#pragma once

#ifdef __unix__
#include <SDL2/SDL.h>
#include "GameLoop.h"
#elif defined(_WIN32) || defined(WIN32)
#include <SDL.h>
#include "GameLoop.h"
#endif

// THIS IS SUPPOSED TO BE A SINGLETON!!!
class InputManager
{
    public:
        static InputManager& Instance()
        {
            static InputManager* _instance = new InputManager();
            return *_instance;
        }

        virtual ~InputManager();

        void ProcessInput(GameLoop* g);


        // Read-only variables for everyone!
        const float& x() const { return _x; }
        const float& y() const { return _y; }
        const bool& b1() const { return _b1; }
		const bool& b2() const { return _b2; }

		bool reloadResource();	// It's not const bc that way the variable inside can be turned off after 1 call to this function!

    protected:

    private:
        InputManager();

		// FOR JOYSTICK USE
		SDL_GameController* controller;
		int numJoysticks = 0;

        // This is the virtual controller input per-se!
        // First: joystick
        float _x = 0;
        float _y = 0;       // Both are -1.0 to 1.0
        // NOTE: neg. is left or up, pos. is right or down

        // Next: action buttons
        bool _b1 = false;       // Attack or Action
        bool _b2 = false;       // Dash or Jump?? (I dont really know...)

		// TEST: For debugging
		bool _reloadRes = false;	// This is the 'r' button which will trigger a 'reload the room' signal


        // Well, I only use these functions eh.
        // FOR KEYBOARD USE
        bool kLeft = false, kRight = false, kUp = false, kDown = false;
};
