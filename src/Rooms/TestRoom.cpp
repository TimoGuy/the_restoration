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


#define FADE_IN_OUT_TICKS 20

Quad* screenTransition;			// This will be a fade in AND out thing, okay?



TestRoom::TestRoom(std::string name, GameLoop* gloop, int playerGX, int playerGY, bool fadeIn, SDL_Color fadeInColor) : Room(gloop)
{
	// You want a fade-in there boi???
	if (fadeIn)
	{
		ScreenFadeIn(fadeInColor);		// Here you go!
	}

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

	delete screenTransition;
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




void TestRoom::ScreenTransition(const std::function<void()>& lambda, SDL_Color fadeOutColor)
{
	scrTransLambdas.push_back(lambda);
	if (screenTransition == NULL)
	{
        int w, h;
		_gloop->GetWindowDimensions(w, h);
		screenTransition = new Quad(w, h);
    }
	scrTransColor = fadeOutColor;

	// So,,,, like,,,, I need to show the game there's fade out business, I suppose.
	fadeOutTimer = FADE_IN_OUT_TICKS;
}

void TestRoom::ScreenFadeIn(SDL_Color fadeOutColor)
{
	if (screenTransition == NULL)
	{
        int w, h;
		_gloop->GetWindowDimensions(w, h);
		screenTransition = new Quad(w, h);
    }
	scrTransColor = fadeOutColor;

	// Same drill!
	fadeInTimer = FADE_IN_OUT_TICKS;
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
    camX = camFocusObj->getX();
    camY = camFocusObj->getY();

    // Cap it to the center if it's too large
    bool lockX = false, lockY = false;
    int w = SCREEN_WIDTH / GRID_SIZE,
        h = SCREEN_HEIGHT / GRID_SIZE;
    if (w >= gWidth)
	{
        lockX = true;
        camX = gWidth * GRID_SIZE;
	}
	if (h >= gHeight)
	{
		lockY = true;
		camY = gHeight * GRID_SIZE;
	}



	// Clamp it to the edges (reuse the variables)
	_gloop->GetWindowDimensions(w, h);
	if (!lockX)
	{
		camX = std::max(w / 2.0f, std::min(camX, (float)gWidth * GRID_SIZE));
	}
	if (!lockY)
	{
		camY = std::max(h / 2.0f, std::min(camY, (float)gHeight * GRID_SIZE));
	}


	// Offset the camera!!!
#define CAM_MOVE_MULTIPLIER 15
#define CAM_SCALE_MULTIPLIER 0.05f
	if (InputManager::Instance().b3())
	{
		// Move!!!!
		camOffX += InputManager::Instance().x() / camScale * CAM_MOVE_MULTIPLIER;
		camOffY += InputManager::Instance().y() / camScale * CAM_MOVE_MULTIPLIER;

		// Scale!!
		if (InputManager::Instance().b1())
		{
			camScale -= CAM_SCALE_MULTIPLIER;	// Zoom out
		}
		else if (InputManager::Instance().b2())
		{
			camScale += CAM_SCALE_MULTIPLIER;	// Zoom in
		}

		camScale = std::max(0.01f, camScale);
	}
	else
	{
		// Reset the camera since you let go of manipulation button
		camOffX = camOffY = 0;
		camScale = 1;
	}

	// Update the camera
	camX += camOffX;
	camY += camOffY;
}

void TestRoom::Render()
{
	glLoadIdentity();

	// Set the camera's zoom
	{
		float doX = camX + (SCREEN_WIDTH / 2);
		float doY = camY + (SCREEN_HEIGHT / 2);
		glScalef(camScale, camScale, camScale);
		glTranslatef(-doX, -doY, 0.0f);
	}

	// Set camera position
	//glTranslatef(-camX, -camY, 0.0f);
	glTranslatef(SCREEN_WIDTH / 2 / camScale, SCREEN_HEIGHT / 2 / camScale, 0);




	// Call a render for everyone!
	for (int it = 0; it < gameObjects.size(); ++it)
	{
		gameObjects.at(it)->Render();
	}



	// If there's a screen transition, do it!
	if (fadeOutTimer >= -1)						// The -1 is so that while it's 0 the GameLoop actually gets to get up to GL_Swap()
	{
		// Reset stuff!
		glLoadIdentity();

		// Screen will turn black (or whatever color you chose)
		float alpha = (FADE_IN_OUT_TICKS - fadeOutTimer) / (float)FADE_IN_OUT_TICKS;

		glColor4f(scrTransColor.r / 255.0f, scrTransColor.g / 255.0f, scrTransColor.b / 255.0f, alpha);

		int w, h;
		_gloop->GetWindowDimensions(w, h);
		screenTransition->Render(-w / 2, -h / 2);

		// Tick down
		fadeOutTimer--;

		// Check if complete!
		if (fadeOutTimer < -1)		// So it was just at -1, then ticked down to -2 (theoretically), it is complete.
		{
			// Just run all the lambdas, don't quit!! Be made quit!!
			for (int i = scrTransLambdas.size() - 1; i >= 0; i--)
			{
				// Go backwards so we can delete too!
				auto lambda = scrTransLambdas.at(i);
				lambda();

				// Delete!
				scrTransLambdas.erase(scrTransLambdas.begin() + i);
			}

			// And, here's the thing, this function won't run
			// again until someone takes the fadeOutTimer and
			// sets it to some number again.
		}
	}
	else if (fadeInTimer >= 0)			// There's no heavy lambdas being executed here, so 0's just fine!
	{
		// Reset stuff!
		glLoadIdentity();

		// Screen will turn from black (or the color eh) to clear
		float alpha = fadeInTimer / (float)FADE_IN_OUT_TICKS;

		// Render!!!
		glColor4f(scrTransColor.r / 255.0f, scrTransColor.g / 255.0f, scrTransColor.b / 255.0f, alpha);

		int w, h;
		_gloop->GetWindowDimensions(w, h);
		screenTransition->Render(-w / 2, -h / 2);

		// Tick!
		fadeInTimer--;
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

