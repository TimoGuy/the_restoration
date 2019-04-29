#pragma once

#ifdef __unix__
#include <SDL2/SDL.h>
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

    protected:

    private:
        SDL_Window* _window;
        bool _running = true;
};
