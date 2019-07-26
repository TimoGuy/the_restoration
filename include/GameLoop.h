#pragma once

#if defined(__unix__) || defined(__APPLE__)
#include <SDL2/SDL.h>
#elif defined(_WIN32) || defined(WIN32)
#include <SDL.h>
#endif

class Room;




class GameLoop
{
    public:
        GameLoop(SDL_Window*);
        bool Execute();
        virtual ~GameLoop();

        void TerminateLoop() { _running = false; }

        void SetRoom(Room* newRoom);
        Room* GetRoom();

        void AddPause(int numTicks);

        void SetWindowDimensions(int width, int height);
        void GetWindowDimensions(int& width, int& height);          // Dipstick function!!!!
		bool DidJustResize();			// This will be on for only 1 tick really

    protected:

    private:
        SDL_Window* _window;
        bool _running = true;

        int waitTicks = 0;

        int _windowWidth, _windowHeight;
		bool _didJustResize;



        // This is the current room
        Room* currentRoom;

        static void LoadGameVariables();
};
