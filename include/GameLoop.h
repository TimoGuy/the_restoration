#pragma once

#ifdef __unix__
#include <SDL2/SDL.h>
#include "Room.h"
#elif defined(_WIN32) || defined(WIN32)
#include <SDL.h>
#include "Rooms\Room.h"
#endif

class GameLoop
{
    public:
        GameLoop(SDL_Window*);
        bool Execute();
        virtual ~GameLoop();

        void TerminateLoop() { _running = false; }

        void SetRoom(Room* newRoom);
        Room* GetRoom();

        void SetWindowDimensions(int width, int height);
        void GetWindowDimensions(int& width, int& height);          // Dipstick function!!!!
		bool DidJustResize();			// This will be on for only 1 tick really



        // Variables controlling the game
        static bool sawTutorial;

    protected:

    private:
        SDL_Window* _window;
        bool _running = true;

        int _windowWidth, _windowHeight;
		bool _didJustResize;



        // This is the current room
        Room* currentRoom;
};
