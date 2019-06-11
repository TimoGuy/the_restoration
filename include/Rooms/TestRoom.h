#pragma once

#ifdef __unix__
#include "Rooms/Room.h"
#include "Entity.h"
#include "defs.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Rooms/Room.h"
#include "../../include/Object.h"
#include "../../include/defs.h"
#endif

#include <stdio.h>
#include <iostream>
#include <vector>

// Hopefully this is where 90% of the game
// will happen! Load a file via IO and then go!

class TestRoom : public Room
{
    public:
        TestRoom(std::string name);
        virtual ~TestRoom();


        Object** getCollisionMap() { return collisionMap; }
		std::vector<Entity*>* getEntityList() { return &entityList; }
        int getGWidth() { return gWidth; }
        int getGHeight() { return gHeight; }


        void Update();
        void Render();

        bool SwitchLevelIO(std::string name);       // This is the thing after the "n_"
		void RequestLevelSwitch(std::string name);	// This should be what the user uses!!!
		void RequestLevelSwitch(std::string name, int playerGX, int playerGY);	// This should be what the user uses!!!

    protected:

    private:
		void Destruct();


        Object** collisionMap;      // This will be an array
		std::vector<Entity*> entityList;	// Stores all the entities inside of the room!
        int gWidth, gHeight;        // The grid's width and height




        std::vector<Object*> gameObjects;


        // Level-loading
        std::string currentLvl;
		std::string currentLvlFilename;
        std::vector<std::string> tokens;        // These are the level tokens which dictate its objects within (eg: an exit object's params)

		std::string pleaseSwitchLevelsToThisOne;	// Hopefully this is initialized as empty...
		void SwitchLevelAndSetUpLevelForPlayer(std::string name);	// This switches and then loads the new room
        static std::string FindLevelIO(std::string name, std::string dir);  // the name of the level (not filename (w/out the 'n_')), and the directory of the levels!

        // For the camera
        float camX = 0, camY = 0;   // Make this the same coords as the player!
        Object* camFocusObj;        // This is the pointer to the player, eh!

        // For custom entering
        bool customEnter = false;
        int ceGX, ceGY;
};
