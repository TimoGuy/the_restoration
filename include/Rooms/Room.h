#pragma once

#ifdef __unix__
#include "Base.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../Base.h"
#endif

class GameLoop;

class Room : public Base
{
    public:
        Room(GameLoop* gloop);
        virtual ~Room();

        virtual void Update() = 0;
        virtual void Render() = 0;

        GameLoop* GetGameLoop();

    protected:
        GameLoop* _gloop;

    private:
};
