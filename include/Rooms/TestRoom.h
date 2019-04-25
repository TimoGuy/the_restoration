#pragma once

#include "Rooms/Room.h"
#include "Object.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include "defs.h"

// Hopefully this is where 90% of the game
// will happen! Load a file via IO and then go!

class TestRoom : public Room
{
    public:
        TestRoom();
        virtual ~TestRoom();

        void Update();
        void Render();

        void SwitchLevelIO(std::string name);       // This is the thing after the "n_"

    protected:

    private:
        std::vector<Object*> gameObjects;


        // Level-loading
        std::string currentLvl;

        static std::string FindLevelIO(std::string name, std::string dir);  // the name of the level (not filename), and the directory of the levels!

        // For the camera
        float camX = 0, camY = 0;   // Make this the same coords as the player!
        Object* camFocusObj;        // This is the pointer to the player, eh!
};
