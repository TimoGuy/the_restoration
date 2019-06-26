#ifdef __unix__
#include "InputManager.h"
#include "TestRoom.h"
#include "Players/TestGameObj.h"
#include "Trigger.h"
#include "Lib/Texture.h"
#include <unistd.h>
#include <dirent.h>
#include "ObjectFactory.h"
#include <SDL2/SDL_opengl.h>
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/InputManager.h"
#include "../../include/Rooms/TestRoom.h"
#include "../../include/Players/TestGameObj.h"
#include "../../include/Players/Trigger.h"
#include "../../include/Lib/Texture.h"
//#include <io.h>
#include "../../include/Lib/dirent.h"
#include "../../include/ObjectFactory.h"
#include <SDL_opengl.h>
#endif

#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <string>

TestRoom::TestRoom(std::string name, GameLoop* gloop, int playerGX, int playerGY) : Room(gloop)
{
    // Tear down all the objects in the object list
	for (int it = 0; it != gameObjects.size(); ++it)
	{
		delete gameObjects.at(it);
	}
	gameObjects.clear();


	// Init
	camFocusObj == NULL;



	// TEST CODE to load a level
	bool success;
	success = LoadLevelIO(name);

	if (!success)
	{
		printf("ERROR: Room was not created, because level switching did not succeed\n");
		return;
	}

	// To set up the player
	for (int i = 0; i < gameObjects.size(); i++)
	{
		// Look for the player!!!
		if (dynamic_cast<TestGameObj*>(gameObjects.at(i)) != NULL)
		{
			camFocusObj = gameObjects.at(i);

			// Sort the player to the very back
			// This way, the player will always be updated late!
			gameObjects.erase(gameObjects.begin() + i);
			gameObjects.push_back(camFocusObj);
			printf("\n\n\n\t\tPlayer was pushed_back() and level is loaded!!!\n");

			if (playerGX >= 0 && playerGY >= 0)
			{
                // Change the coords
                ((TestGameObj*)camFocusObj)->SetGridCoords(playerGX, playerGY);
                ((TestGameObj*)camFocusObj)->UpdateStartCoords();
                printf("\t\t\tPlayer set to custom coords %i,%i\n", playerGX, playerGY);
			}
			break;
		}
	}


    // To set up the triggers!
	for (int i = 0; i < gameObjects.size(); i++)
	{
		// Look for a trigger, tigger!!!
		if (dynamic_cast<Trigger*>(gameObjects.at(i)) != NULL)
		{
            // Found!!!
            Trigger* tmpTrigger = (Trigger*)gameObjects.at(i);

            if (tmpTrigger->NeedsSetup())   // If already setup, then will skip
            {
                // Get the first 't' code from the params
                int pos = -1;
                for (int i = 0; i < rmParams.size(); i++)
                {
                    if (rmParams.at(i) == std::string("t"))
                    {
                        // Start here and just start reading the string thru to get values!
                        pos = i;
                        break;
                    }
                }

                if (pos < 0)
                {
                    // Break out and print error
                    printf("\n\nERROR:: Not enough \'t\' params in level to create an exit...\n\n\n");
                    break;
                }


                // Grab values
                bool touchTrigger;
                    std::istringstream(rmParams.at(pos + 1)) >> touchTrigger;
                std::string eventTagType = rmParams.at(pos + 2);
                std::string eventName = rmParams.at(pos + 3);

                bool custEntr = false;
                std::string custEntrCoords;
                if (eventTagType == "n" &&                             // Check if a game level type
                    pos + 4 < rmParams.size() &&                   // If there's a fourth param (this'd be the coords for a cust. entrance of player)
                    rmParams.at(pos + 4) != std::string("t"))
                {
                    custEntr = true;
                    custEntrCoords = rmParams.at(pos + 4);
                }


                // Set up the trigger object to be a master, and it will find all its slaves.
                tmpTrigger->SetEventIDAndSetMaster(eventTagType + "_" + eventName, touchTrigger);

                int end = 4;
                if (custEntr)
                {
                    end = 5;    // A fifth param...             (it's the custom entrance coords!!!)

                    // Parse from the x value (i.e. has a 123x456 to display coords)
                    int ceGX, ceGY;

                    std::string token;
                    std::istringstream tokenStream(custEntrCoords);
                    int times = 0;
                    while (std::getline(tokenStream, token, 'x') &&
                        times <= 1)
                    {
                        if (times == 0)     // X axis
                        {
                            std::istringstream(token) >> ceGX;
                        }
                        else if (times == 1)    // Y axis
                        {
                            std::istringstream(token) >> ceGY;
                        }

                        times++;
                    }

                    printf("\n\n\tCustom entrance for player at %i,%i\n\n\n", ceGX, ceGY);
                    tmpTrigger->SetEntranceCoords(ceGX, ceGY);
                }
                else
                {
                    printf("\tNo custom exit code found\n");
                }

                // Remove those values for future params-checking!
                rmParams.erase(rmParams.begin() + pos, rmParams.begin() + pos + end);
            }
		}
	}
}





TestRoom::~TestRoom()
{
	// Tear down all the objects in the object list
	// (no need for the entity list to be touched eh)
	for (int it = 0; it != gameObjects.size(); ++it)
	{
		delete gameObjects.at(it);
	}
	gameObjects.clear();
}



void TestRoom::RemoveEntity(Entity* thisObj)
{
    // Remove from entity list
    entityList.erase(
        std::remove(entityList.begin(),
            entityList.end(), thisObj),
            entityList.end());

    // And from gameObject list too!
    gameObjects.erase(
        std::remove(gameObjects.begin(),
            gameObjects.end(), thisObj),
            gameObjects.end());
}




void TestRoom::Update()
{
    // Update all objects
    for (int it = 0; it < gameObjects.size(); ++it)
    {
        gameObjects.at(it)->Update();
    }







    // Is there cam?
    if (camFocusObj == NULL)
        return;

    // Update camera
    camX = camFocusObj->getX() - (SCREEN_WIDTH / 2);
    camY = camFocusObj->getY() - (SCREEN_HEIGHT / 2);

    // Cap it to the center if it's too large
    bool lockX = false, lockY = false;
    int w = SCREEN_WIDTH / GRID_SIZE,
        h = SCREEN_HEIGHT / GRID_SIZE;
    if (w >= gWidth)
    {
        lockX = true;
        camX = ((gWidth * GRID_SIZE) - SCREEN_WIDTH) / 2;
    }
    if (h >= gHeight)
    {
        lockY = true;
        camY = ((gHeight * GRID_SIZE) - SCREEN_HEIGHT) / 2;
    }



    // Clamp it to the edges
    if (!lockX)
    {
        camX = std::max(0, std::min((int)camX, gWidth * GRID_SIZE - SCREEN_WIDTH));
    }
    if (!lockY)
    {
        camY = std::max(0, std::min((int)camY, gHeight * GRID_SIZE - SCREEN_HEIGHT));
    }
}

void TestRoom::Render()
{
    // Set the camera
    glLoadIdentity();
    glTranslatef(-camX, -camY, 0.0f);

    // Call a render for everyone!
    for (int it = 0; it < gameObjects.size(); ++it)
    {
        gameObjects.at(it)->Render();
    }
}





bool TestRoom::LoadLevelIO(std::string name)
{
    // Find/Search for the requested level (within the 'n_...' form-factor)
#ifdef __unix__
    std::string currentDir = std::string(get_current_dir_name()) + std::string("/.data/levels/");
#elif defined(_WIN32) || defined(WIN32)
	std::string currentDir = std::string(".data/levels/");
#endif

    printf("Levels are stored in:\n%s\n\n", currentDir.c_str());
    std::string levelFilename = TestRoom::FindLevelIO(name, currentDir);
    if (levelFilename.empty())           // Error checking....
    {
        printf("ERROR: Unable to switch levels... no file was found with the level name \"%s\"\n", name.c_str());
        return false;
    }

    // Load a level
    int comp;
    const std::string& fileName = currentDir + levelFilename;
    int req = STBI_rgb;
    unsigned char* imgData = stbi_load(fileName.c_str(), &gWidth, &gHeight, &comp, req);

    if (imgData == NULL)
    {
        printf("Error! Texture loading failed for \"%s\"\n", fileName.c_str());
        return false;
    }

    // Initialize / empty the token/params list
    rmParams.clear();

	// Remember what level you're on!
	currentLvl = name;
	currentLvlFilename = levelFilename;

	// Cut off the beginning I suppose!
	std::string prefixCutoff = currentLvl + std::string(".");
	int len = currentLvlFilename.size() - prefixCutoff.size() - 4;                  // The '4's referring to the '.png' at the end
	std::string levelParams = currentLvlFilename.substr(prefixCutoff.size(), len);

	// This way we can parse and get the code from the filename!
    std::string token;
    std::istringstream tokenStream(levelParams);

    printf("\nPrinting out level param data...\n");
    while (std::getline(tokenStream, token, '_'))
    {
        printf("%s\n", token.c_str());
        rmParams.push_back(token);        // Add into level's params to be used l8r!!
    }
    printf("\n\n\n\n");




    // Initialize the Collision Map as EMPTY!!!!
    collisionMap = new Object*[gWidth * gHeight];
    for (int uu = 0; uu < gWidth * gHeight; uu++)
    {
        collisionMap[uu] = NULL;
    }

    // Yeah so hopefully you kept 'req' at STBI_rgb,
    // bc we're gonna use the 3 values to get the objects
    // for the room from ObjectFactory.h
    //
    // convert the pixels to objects yall!
    int i = 0;
    while (i < gWidth * gHeight)
    {
        // We'll assume it's STBI_rgb
        std::string out_string;
        std::stringstream ss[3];
        ss[0] << (int)imgData[(i * 3)];
        ss[1] << (int)imgData[(i * 3) + 1];
        ss[2] << (int)imgData[(i * 3) + 2];


        std::string colorId =
            ss[0].str() + std::string(",") +
            ss[1].str() + std::string(",") +
            ss[2].str();

        Object* _new = ObjectFactory::GetObjectFactory().Build(colorId.c_str(), (int)(i % gWidth), (int)(i / gWidth), this);
        if (_new != NULL)
            gameObjects.push_back(_new);        // Adds the returned built object!

        i++;
    }





    // Free loaded image
    stbi_image_free(imgData);
	return true;
}













std::string TestRoom::FindLevelIO(std::string name, std::string dir)         // The level has to be a png file in the levels folder
{
    // Search using dirent.h??? (what is that, anyways??)
    DIR* dirPoint = opendir(dir.c_str());
    dirent* entry = readdir(dirPoint);

    std::string searchCriteria = name + std::string(".");

    while (entry)
    {
        // Only searches the top level
        if (entry->d_type == DT_REG)
        {
            // REGULAR FILE!!! Check if it matches
            std::string fname = entry->d_name;
            if (strncmp(fname.c_str(), searchCriteria.c_str(), searchCriteria.size()) == 0)         // This sees if it 'startswith' the searchCrit!
            {
                // Spit it out!
                printf("Found level: %s\n\n", fname.c_str());
                return fname;
            }
        }

        entry = readdir(dirPoint);
    }


	printf("ERROR: Could not find level \"%s\"\n", name.c_str());
    return std::string();
}

