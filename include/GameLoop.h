#pragma once

#include <SDL2/SDL.h>

class GameLoop
{
    public:
        GameLoop(SDL_Window*);
        bool Execute();
        virtual ~GameLoop();

        void TerminateLoop() { _running = false; }

    protected:

    private:
        SDL_Window* _window;
        bool _running = true;
};
