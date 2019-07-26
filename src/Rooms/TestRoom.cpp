#if defined(__unix__) || defined(__APPLE__)
#include "InputManager.h"
#include "TestRoom.h"
#include "Players/TestGameObj.h"
#include "Trigger.h"
#include "Lib/Texture.h"
#include "ObjectFactory.h"
#include <SDL2/SDL_opengl.h>
#include "GameLoop.h"
#include "TileSet.h"
#include "Quad.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/InputManager.h"
#include "../../include/Rooms/TestRoom.h"
#include "../../include/Players/TestGameObj.h"
#include "../../include/Players/Trigger.h"
#include "../../include/Lib/Texture.h"
#include "../../include/ObjectFactory.h"
#include <SDL_opengl.h>
#include "../../include/GameLoop.h"
#include "../../include/Rooms/TileSet.h"
#include "../../include/Shape/Quad.h"

#endif

#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <string>


#define FADE_IN_OUT_TICKS 20

Quad* screenTransition;			// This will be a fade in AND out thing, okay?

Quad* oneStamina;

TestRoom::TestRoom(std::string name, GameLoop* gloop, int playerGX, int playerGY, bool fadeIn, SDL_Color fadeInColor) : Room(gloop)
{
    roomTileSet = new TileSet();

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
	camFocusObj = NULL;



	// TEST CODE to load a level
	currentLvl = name;          // Load for serialization
	bool success;
	success = LoadLevelIO(name);

	if (!success)
	{
		printf("ERROR: Room was not created, because level switching did not succeed\n");
		return;
	}





	// To set up the player
	for (unsigned int i = 0; i < gameObjects.size(); i++)
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

			// Update the camera to reflect all the wonderful changes!
            camX = camFocusObj->getX();
            camY = camFocusObj->getY();
			break;
		}
	}


    // To set up the triggers!
	for (unsigned int i = 0; i < gameObjects.size(); i++)
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
                for (unsigned int i = 0; i < rmParams.size(); i++)
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
                std::string eventName = rmParams.at(pos + 2);



				// Set up the trigger object to be a master, and it will find all its slaves.
				tmpTrigger->SetEventIDAndSetMaster(eventName, touchTrigger);
							// This needs to be set up before we start setting properties to it, like custom coords!



				int end = 3;		// This is left at 3 if no custom coords
                if (eventName.at(0) == 'n' &&                             // Check if a game level type
                    pos + 3 < (signed int)rmParams.size() &&                   // If there's a fourth param (this'd be the coords for a cust. entrance of player)
                    rmParams.at(pos + 3) != std::string("\n"))
                {
                    end = 4;    // A 4th param...             (it's the custom entrance coords!!!)

					// Grab value
					std::string custEntrCoords = rmParams.at(pos + 3);


                    // Parse from the x value (i.e. has a 123x456 to display coords)
                    int ceGX, ceGY;

                    std::string token;
                    std::istringstream tokenStream(custEntrCoords);

					std::getline(tokenStream, token, ',');
					std::istringstream(token) >> ceGX;		// X coord
					std::getline(tokenStream, token, ',');
					std::istringstream(token) >> ceGY;		// Y coord



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
	delete oneStamina;
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
	scrTransColor = fadeOutColor;

	// So,,,, like,,,, I need to show the game there's fade out business, I suppose.
	fadeOutTimer = FADE_IN_OUT_TICKS;
}

void TestRoom::ScreenFadeIn(SDL_Color fadeOutColor)
{
	scrTransColor = fadeOutColor;

	// Same drill!
	fadeInTimer = FADE_IN_OUT_TICKS;
}



float camHsp = 0, camVsp = 0;

void TestRoom::Update()
{
    // Update all objects
    for (unsigned int it = 0; it < gameObjects.size(); ++it)
    {
        gameObjects.at(it)->Update();
    }







    // Is there cam?
    if (camFocusObj == NULL)
        return;

    // Update camera

    // DEBUG:: do a fancy camera trick!
    if (InputManager::Instance().b4())
    {
        camX = camFocusObj->getX();
        camY = camFocusObj->getY();
    }
    else
    {
        camX += (camFocusObj->getX() - camX) / 10.0f + camReqOffX;
        camY += (camFocusObj->getY() - camY) / 10.0f + camReqOffY;
    }

    // Reset the requested offset cam now!
    camReqOffX = camReqOffY = 0;

    // Cap it to the center if it's too large
    bool lockX = false, lockY = false;
    int w = SCREEN_WIDTH / GRID_SIZE,
        h = SCREEN_HEIGHT / GRID_SIZE;
    if (w >= gWidth)
	{
        lockX = true;
        camX = (float)gWidth * GRID_SIZE;
	}
	if (h >= gHeight)
	{
		lockY = true;
		camY = (float)gHeight * GRID_SIZE;
	}



	// Clamp it to the edges (reuse the variables)
	_gloop->GetWindowDimensions(w, h);
	if (!lockX)
	{
		camX = std::max(w / 2.0f, std::min(camX, (float)gWidth * GRID_SIZE - w / 2.0f));
	}
	if (!lockY)
	{
		camY = std::max(h / 2.0f, std::min(camY, (float)gHeight * GRID_SIZE - h / 2.0f));
	}


	// Offset the camera!!!
#define CAM_MOVE_MULTIPLIER 2.5
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

	// Update the debug mvtment camera
	camX += camOffX;
    camY += camOffY;
}

void TestRoom::Render()
{
	glLoadIdentity();

	// Set the camera's zoom
	glScalef(camScale, camScale, camScale);
	glTranslatef(-camX, -camY, 0.0f);





	// And then the tileset underlay!!!!!
	glColor4f(1, 1, 1, 1);
	roomTileSet->RenderVerts();

	// Call a render for everyone!
	for (unsigned int it = 0; it < gameObjects.size(); ++it)
	{
		gameObjects.at(it)->Render();
	}






	// Reset stuff!
	glLoadIdentity();

	// Now render the HUD!!!
#define ONE_STAMINA_SIZE 24
#define ONE_STAMINA_PADDING 8
	if (oneStamina == NULL)
	{
		oneStamina = new Quad(ONE_STAMINA_SIZE, ONE_STAMINA_SIZE);
	}

	int stamGauges = ((TestGameObj*)camFocusObj)->GetNumJumps();
	for (int i = 0; i < stamGauges; i++)
	{
		// Start at the upper left corner and just start drawing them green!
		glColor3f(0, 1, 0);

		int oneUnit = ONE_STAMINA_SIZE + ONE_STAMINA_PADDING;

		int originalX = i * oneUnit;

		float xPos = float(originalX % 1024);
		float yPos = float(originalX / 1024 * oneUnit);

		oneStamina->Render(xPos - 508, yPos - 284);
	}





	// Setup screen transitioner, if needed ;)
	if (screenTransition == NULL ||
		_gloop->DidJustResize())
	{
		int w, h;
		_gloop->GetWindowDimensions(w, h);
		screenTransition = new Quad((float)w, (float)h);
	}


	// If there's a screen transition, do it!
	if (fadeOutTimer >= -1)						// The -1 is so that while it's 0 the GameLoop actually gets to get up to GL_Swap()
	{


		// Screen will turn black (or whatever color you chose)
		float alpha = (FADE_IN_OUT_TICKS - fadeOutTimer) / (float)FADE_IN_OUT_TICKS;

		glColor4f(scrTransColor.r / 255.0f, scrTransColor.g / 255.0f, scrTransColor.b / 255.0f, alpha);
		screenTransition->Render(-screenTransition->GetWidth() / 2, -screenTransition->GetHeight() / 2);

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
		screenTransition->Render(-screenTransition->GetWidth() / 2, -screenTransition->GetHeight() / 2);

		// Tick!
		fadeInTimer--;
	}
}





bool TestRoom::LoadLevelIO(std::string name)
{
	// Initialize / empty the token/params list
	rmParams.clear();


    // Find/Search for the requested level (within the 'n_...' form-factor)
	std::string currentDir = std::string(".data/levels/");

    printf("Levels are stored in:\n%s\n\n", currentDir.c_str());

	// Level variables
	std::string levelFilename;

	// Level's tileset loading prep variable
	std::string levelTileSetFname;			// Should init as an empty string

	// Try loading the level!!!
	std::string line;
	std::ifstream myfile(currentDir + name + ".txt");
	if (myfile.is_open())
	{
		std::cout << "Reading level file:\n";



		// Read line by line
		while (std::getline(myfile, line))
		{
			// DEBUG
			std::cout << line << '\n';

			if (line.size() <= 0 ||         // If this is a blank line or is a comment, then ignore!
				line.at(0) == '#')
			{
				// This is empty!
				continue;
			}

			// Check what data type
			if (line.at(0) == 'i')
			{
				// It's a collision picture!!!! (the base one)
				levelFilename = line.substr(2);			// Cut off the 'i' and '\t'

				// NOTE: this file will later be loaded!!!
				// (@ region "Load Collision Picture")
			}
			else if (line.at(0) == 's')
			{
				// It's the tile-set!!!! (support animation l8r TODO)
				roomTileSet->LoadTileTex(currentDir + line.substr(2));		// It's a rel path!!! (also cutoff the 's' and '\t' too eh)
			}
			else if (line.at(0) == 'm')
			{
				// It's the tile set to collision map!!!!
				levelTileSetFname = line.substr(2);			// Cut off the 'm' and '\t'

				// NOTE: this file will later be loaded!!!
				// (@ region "Load Level Tileset Map")
			}
			else
			{
				// It's some extra level params to pass on!
				std::string token;
				std::istringstream tokenStream(line);

				printf("\nFound a line of level params...\n");
				while (std::getline(tokenStream, token, '\t'))
				{
					printf("\t%s\n", token.c_str());
					rmParams.push_back(token);        // Add into level's params to be used l8r!!
				}

				// Seal off this line! (w/ a line break)
				rmParams.push_back("\n");
			}
		}
	}
	else
	{
		printf("ERROR: Unable to switch levels... no file was found with the level name \"%s\"\n", name.c_str());
		return false;
	}


	// Just error-checking!
	if (!levelFilename.empty())
	{
#pragma region Load Collision Picture

		// Load a picture of the level
		int comp;
		const std::string& fileName = currentDir + levelFilename;
		int req = STBI_rgb;
		unsigned char* imgData = stbi_load(fileName.c_str(), &gWidth, &gHeight, &comp, req);

		if (imgData == NULL)
		{
			printf("Error! Texture loading failed for \"%s\"\n", fileName.c_str());
			return false;
		}



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

#pragma endregion
	}
	else
	{
		printf("WARNING: No filename for collision data was specified... use 'i' in the level file\n");
		// Do nuthin, cuz it's a warning!
	}



	// First check if there was a tileset
	// requested in the first place!!!
	if (!levelTileSetFname.empty())
	{
#pragma region Load Level Tileset Map

		// Load it up!!!!
		int comp;
		const std::string& fileName = currentDir + levelTileSetFname;		// See? Just a little bit different!!!
		int req = STBI_rgb;
		unsigned char* imgData = stbi_load(fileName.c_str(), &gWidth, &gHeight, &comp, req);

		if (imgData == NULL)
		{
			printf("Error! Texture loading failed for \"%s\"\n", fileName.c_str());
			return false;
		}

		// Yeah so hopefully you kept 'req' at STBI_rgb,
		// bc we're gonna use the 3 values to get the objects
		// for the room from ObjectFactory.h
		//
		// convert the pixels to objects yall!
		for (int i = 0; i < gWidth * gHeight; i++)
		{
			// We'll assume it's STBI_rgb (hence 3 multiplier eh)
			roomTileSet->InterpretAndAddVector(i, (int)(i % gWidth), (int)(i / gWidth), gWidth, gHeight, imgData);		// Take the r_value, and it will interpret it!
		}


		// Free loaded image
		stbi_image_free(imgData);

#pragma endregion
	}
	else
	{
		printf("WARNING: No filename for tile-mapping data was specified... use 'm' in the level file\n");
		// Do nuthin, cuz it's a warning!
	}


	return true;
}


void TestRoom::AddCamOffCoords(float offx, float offy)
{
    camReqOffX += offx;
    camReqOffY += offy;
}



std::string TestRoom::GetLevelName()
{
    return currentLvl;
}

