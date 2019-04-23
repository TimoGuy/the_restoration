#pragma once

#include "Rooms/Room.h"
#include "Object.h"


class TestRoom : public Room
{
    public:
        TestRoom();
        virtual ~TestRoom();

        void Update();
        void Render();

    protected:

    private:
        Object* player;
};
