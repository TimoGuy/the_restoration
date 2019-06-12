#pragma once

#ifdef __unix__
#include <SDL2/SDL.h>
#include "Room.h"
#elif defined(_WIN32) || defined(WIN32)
#include <SDL.h>
#endif

class GameLoop
{
    public:
        GameLoop(SDL_Window*);
        bool Execute();
        virtual ~GameLoop();

        void TerminateLoop() { _running = false; }

//        void RequestNewLevel(std::string name);
//        void RequestNewLevel(std::string name, int playerGX, int playerGY);
//        void RequestNewCutscene(std::string name);
        Room* GetRoom();
        void SetRoom(Room* newRoom);

    protected:

    private:
        SDL_Window* _window;
        bool _running = true;



        // This is the current room
        Room* currentRoom;
};
