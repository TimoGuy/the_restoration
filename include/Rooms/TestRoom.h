#pragma once

#ifdef __unix__
#include "Rooms/Room.h"
#include "Entity.h"
#include "defs.h"
#include "TileSet.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Rooms/Room.h"
#include "../../include/Players/Entity.h"
#include "../../include/defs.h"
#include "../../include/Rooms/TileSet.h"
#endif

#include <stdio.h>
#include <iostream>
#include <vector>

// Hopefully this is where 90% of the game
// will happen! Load a file via IO and then go!

class TestRoom : public Room
{
    public:
        TestRoom(std::string name, GameLoop* gloop, int playerGX=-1, int playerGY=-1, bool fadeIn=false, SDL_Color fadeInColor={ 0, 0, 0, 1 });
        ~TestRoom();


        Object** getCollisionMap() { return collisionMap; }
		std::vector<Entity*>* getEntityList() { return &entityList; }
		void RemoveEntity(Entity* thisObj);
        int getGWidth() { return gWidth; }
        int getGHeight() { return gHeight; }

		void ScreenTransition(const std::function<void()>& lambda, SDL_Color fadeOutColor={ 0, 0, 0, 1 });			// This just does the fade out, then runs the lambda eh
		void ScreenFadeIn(SDL_Color fadeOutColor={ 0, 0, 0, 1 });

        void Update();
        void Render();

        void AddCamOffCoords(float offx, float offy);

    protected:

    private:
        Object** collisionMap;      // This will be an array
		std::vector<Entity*> entityList;	// Stores all the entities inside of the room!
        int gWidth, gHeight;        // The grid's width and height




        std::vector<Object*> gameObjects;


		std::vector<std::function<void()>> scrTransLambdas;
		SDL_Color scrTransColor;
		int fadeOutTimer = -100;			// -100 means do nothing
		int fadeInTimer = -100;			// Fade out means screen goes black, fade in is fades into the game screen eh!


        // Level-loading
        std::string currentLvl;
		std::string currentLvlFilename;
        std::vector<std::string> rmParams;        // These are the level tokens which dictate its objects within (eg: an exit object's params)


		bool LoadLevelIO(std::string name);       // This is the thing after the "n_"


		// Level's tileset overlay!!
		// (disable object's rendering and check it out!!!!!)
		TileSet roomTileSet;


        // For the camera
        float camX = 0, camY = 0;   // Make this the same coords as the player!
        float camReqOffX = 0, camReqOffY = 0;       // Objects can override and request camera mvt. (also, these variables are reset to 0 every frame, so you ADD to them in the add())
        Object* camFocusObj = NULL;        // This is the pointer to the player, eh!
		float camOffX = 0,						// For moving the cam manually
			camOffY = 0;
		float camScale = 1;						// For setting size of cam view
};
