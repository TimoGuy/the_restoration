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
#include "Cutscene.h"
#include "SerialManager.h"
#include "StaminaBar.h"


#include "TestMiniboss_Enemy.h"
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
#include "../../include/Rooms/Cutscene/Cutscene.h"
#include "../../include/SerialManager.h"
#include "../../include/ui/StaminaBar.h"

#include "../../include/Players/TestMiniboss_Enemy.h"
#endif

#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <string>


#define FADE_IN_OUT_TICKS 20


struct _BackgroundParallaxObj
{
	Quad* backgroundTex;
	float divisorX;
	float divisorY;
	float xoff;
	float yoff;
};



Quad* screenTransition;			// This will be a fade in AND out thing, okay?

Quad* oneStamina;

TestRoom::TestRoom(std::string name, GameLoop* gloop, int playerGX, int playerGY, bool fadeIn, SDL_Color fadeInColor) : Room(gloop)
{
    roomTileSet = new TileSet();
	roomPropSet = NULL;

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


	// EXTRA DEBUG STUFFF
	gameObjects.push_back(new TestMiniboss_Enemy(168, 81, this));




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




	// Update the cutscene-prop-set if needed
	if (roomPropSet != NULL)
	{
		roomPropSet->Update();
	}
}

StaminaBar* stamBar = new StaminaBar();

void TestRoom::Render()
{
	glLoadIdentity();

	// Set the camera's zoom
	glScalef(camScale, camScale, camScale);
	glTranslatef(-camX, -camY, 0.0f);


	
	// Render the backgrounds
	glColor4f(1, 1, 1, 1);
	int w, h;
	_gloop->GetWindowDimensions(w, h);
	for (unsigned int i = 0; i < backgrounds.size(); i++)
	{
		// Render at the x and y values eh
		float desX = camX / backgrounds.at(i).divisorX + backgrounds.at(i).xoff;
		float desY = camY / backgrounds.at(i).divisorY + backgrounds.at(i).yoff;

		// Iterate until left the screen (x-wrap)
		float offsetX = 0;
		while (desX + offsetX < camX + (w / 2))
		{
			if (desX + offsetX + backgrounds.at(i).backgroundTex->GetWidth() >= camX - (w / 2))
				backgrounds.at(i).backgroundTex->Render(desX + offsetX, desY);
			offsetX += backgrounds.at(i).backgroundTex->GetWidth();
		}
		offsetX = -backgrounds.at(i).backgroundTex->GetWidth();		// now to the left!
		while (desX + offsetX + backgrounds.at(i).backgroundTex->GetWidth() >= camX - (w / 2))
		{
			if (desX + offsetX < camX + (w / 2))
				backgrounds.at(i).backgroundTex->Render(desX + offsetX, desY);
			offsetX -= backgrounds.at(i).backgroundTex->GetWidth();
		}
	}
	




	// And then the tileset underlay!!!!!
	glColor4f(1, 1, 1, 1);
	
	if (roomPropSet != NULL)
	{
		roomPropSet->Render(false);
	}

	roomTileSet->RenderVerts();




	// Call a render for everyone!
	for (unsigned int it = 0; it < gameObjects.size(); ++it)
	{
		gameObjects.at(it)->Render();
	}






	// Reset stuff!
	glLoadIdentity();

	// Now render the HUD!!!
	if (camFocusObj != NULL)
	{
#define ONE_STAMINA_SIZE 24
#define ONE_STAMINA_PADDING 8
		if (oneStamina == NULL)
		{
			oneStamina = new Quad(ONE_STAMINA_SIZE, ONE_STAMINA_SIZE);
		}

		// Render health!!!
		int maxHealth = SerialManager::Instance().GetGameData_Int(
			"player_max_health",
			GAME_VAR_DEF_player_max_health
		);
		int currentHealth = SerialManager::Instance().GetGameData_Int(
			"player_current_health",
			GAME_VAR_DEF_player_current_health
		);

		for (int i = 0; i < maxHealth; i++)
		{
			if (i < currentHealth)
				glColor3f(1, 0, 0);
			else
			{
				glColor3f(0.3f, 0.3f, 0.3f);
			}

			// Draw!
			int oneUnit = ONE_STAMINA_SIZE + ONE_STAMINA_PADDING;
			int xPos = i * oneUnit;
			oneStamina->Render(xPos - 508, -284);
		}


		// Render stamina!!!
		int stamGauges = ((TestGameObj*)camFocusObj)->GetNumJumps();
		stamBar->Render(stamGauges, -508, -284 + ONE_STAMINA_SIZE + ONE_STAMINA_PADDING);
		// for (int i = 0; i < stamGauges; i++)
		// {
		// 	// Start at the upper left corner and just start drawing them green!
		// 	glColor3f(0, 1, 0);

		// 	int oneUnit = ONE_STAMINA_SIZE + ONE_STAMINA_PADDING;

		// 	int originalX = i * oneUnit;

		// 	float xPos = float(originalX % 1024);
		// 	float yPos = float(originalX / 1024 * oneUnit);

		// 	oneStamina->Render(xPos - 508, yPos - 284 + oneUnit);
		// }
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
    // Find/Search for the requested level (within the 'n_...' form-factor)
	std::string currentDir = std::string(".data/levels/");

    printf("Levels are stored in:\n%s\n\n", currentDir.c_str());

	// Try loading the level!!!
	std::ifstream myfile(currentDir + name + ".json");
	Json::Reader reader;
	Json::Value lvlData;
	if (reader.parse(myfile, lvlData))
	{
		// Load collision picture ("entities")
		// (I'm expecting it's there!!!)
		if (lvlData["textures"].isMember("entities") &&
			!lvlData["textures"]["entities"].isNull())
		{
#pragma region Load Collision Picture
			// Load a picture of the level
			int comp;
			const std::string& fileName =
				currentDir +
					lvlData["textures"]["entities"].asString();
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


		// Load "tileset_map" if not null eh
		if (lvlData["textures"].isMember("tileset_map") &&
			!lvlData["textures"]["tileset_map"].isNull())
		{
#pragma region Load Tileset Sprite Sheet
			if (lvlData["textures"].isMember("tileset_spritesheet") &&
				!lvlData["textures"]["tileset_spritesheet"].isNull())
			{
				// Load in that texture!!!!
				roomTileSet->LoadTileTex(
					currentDir +
						lvlData["textures"]["tileset_spritesheet"].asString()
				);
			}
			else
			{
				// Show warning
				printf("WARNING: although \"tileset_map\" was found, \"tileset_spritesheet\" was not found\n");
			}
#pragma endregion

#pragma region Load Level Tileset Map
			{
				// Load it up!!!!
				int comp;
				const std::string& fileName =
					currentDir +
						lvlData["textures"]["tileset_map"].asString();		// See? Just a little bit different!!!
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
			}
#pragma endregion
		}
	}
	else
	{
		printf("ERROR: Unable to switch levels... no file was found with the level name \"%s\"\n", name.c_str());
		return false;
	}
	
	// Setup all the triggers
	if (lvlData.isMember("triggers") &&
		lvlData["triggers"].size() > 0)
	{
#pragma region Setup Triggers
		// To set up the triggers!
		int triggerCount = 0;
		for (unsigned int i = 0;
			i < gameObjects.size() && triggerCount < lvlData["triggers"].size();
			i++)
		{
			// Look for a trigger, tigger!!!
			if (dynamic_cast<Trigger*>(gameObjects.at(i)) != NULL)
			{
				// Found!!!
				Trigger* tmpTrigger = (Trigger*)gameObjects.at(i);

				if (tmpTrigger->NeedsSetup())   // If already setup, then will skip
				{
					// Check if a dead trigger
					if (lvlData["triggers"][std::to_string(triggerCount)].isNull())
					{
						// It's requested to be a dead trigger eh
						tmpTrigger->DisableMe();

						// Set up the trigger object to be a master, and it will find all its slaves.
						tmpTrigger->SetEventIDAndSetMaster("null", false);	
					}
					else
					{
						// Set up the trigger object to be a master, and it will find all its slaves.
						tmpTrigger->SetEventIDAndSetMaster(
							lvlData["triggers"][std::to_string(triggerCount)]["fire_event"].asString(),
							lvlData["triggers"][std::to_string(triggerCount)]["auto_trigger"].asBool()
						);

						if (lvlData["triggers"][std::to_string(triggerCount)]
								.isMember("player_start_coords"))
						{
							printf("\n\n\n\n\n\n\n\t\tSETUP CUSTOM COORDS AT: ");
							// Setup the custom coords too!
							int ceGX = lvlData["triggers"][std::to_string(triggerCount)]["player_start_coords"]["x"].asInt(),
								ceGY = lvlData["triggers"][std::to_string(triggerCount)]["player_start_coords"]["y"].asInt();
							printf("\n\n\tCustom entrance for player at %i,%i\n\n\n", ceGX, ceGY);
							tmpTrigger->SetEntranceCoords(ceGX, ceGY);
						}
					}

					// Update trigger count!
					triggerCount++;
				}
			}
		}
#pragma endregion
	}

	// Backgrounds loading
	if (lvlData.isMember("backgrounds"))
	{
		for (int i = 0; i < lvlData["backgrounds"].size(); i++)
		{
			// It's a background!!!!
			std::string path = currentDir +
				lvlData["backgrounds"][std::to_string(i)]["image"].asString();
			Texture* temp =
				new Texture(
					path.c_str(),
					STBI_rgb_alpha
				);

			_BackgroundParallaxObj newObj;
			newObj.backgroundTex =
				new Quad(temp->GetWidth(), temp->GetHeight(), temp);
			newObj.divisorX = lvlData["backgrounds"][std::to_string(i)]["x_divisor"].asFloat();
			newObj.divisorY = lvlData["backgrounds"][std::to_string(i)]["y_divisor"].asFloat();
			newObj.xoff = lvlData["backgrounds"][std::to_string(i)]["xoff"].asFloat();
			newObj.yoff = lvlData["backgrounds"][std::to_string(i)]["yoff"].asFloat();
			backgrounds.push_back(newObj);
		}
	}


	// Propsets (cutscene)
	if (lvlData.isMember("propset"))
	{
		// It's the prop set (created by a cutscene file)!!!!
		roomPropSet = new Cutscene(lvlData["propset"].asString(), _gloop);
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

