#ifdef __unix__
#include "TestRoom.h"
#include "Players/TestGameObj.h"
#include "Lib/Texture.h"
#include <unistd.h>
#include <dirent.h>
#include "ObjectFactory.h"
#include <SDL2/SDL_opengl.h>
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/InputManager.h"
#include "../../include/Rooms/TestRoom.h"
#include "../../include/Players/TestGameObj.h"
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

TestRoom::TestRoom()
{
	SwitchLevelAndSetUpLevelForPlayer("jojo");
}

TestRoom::~TestRoom()
{
	Destruct();
}

void TestRoom::Update()
{
    // Update all objects
    for (int it = 0; it != gameObjects.size(); ++it)
    {
        gameObjects.at(it)->Update();
    }









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
//#ifdef __unix__
        camX = std::max(0, std::min((int)camX, gWidth * GRID_SIZE - SCREEN_WIDTH));
//#elif defined(_WIN32) || defined(WIN32)
		//camX = max(0, min((int)camX, gWidth * GRID_SIZE - SCREEN_WIDTH));
//#endif
    }
    if (!lockY)
    {
//#ifdef __unix__
        camY = std::max(0, std::min((int)camY, gHeight * GRID_SIZE - SCREEN_HEIGHT));
//#elif defined(_WIN32) || defined(WIN32)
//		camY = max(0, min((int)camY, gHeight * GRID_SIZE - SCREEN_HEIGHT));
//#endif
    }
}

void TestRoom::Render()
{
    // Set the camera
    glLoadIdentity();
    glTranslatef(-camX, -camY, 0.0f);

    // Call a render for everyone!
    for (int it = 0; it != gameObjects.size(); ++it)
    {
        gameObjects.at(it)->Render();
    }



	// FOR DEBUG: check if the room needs to be reloaded
	if (InputManager::Instance().reloadRoom())		// This is a check, but after 1 check the inside variable turns off, so no worries.
	{
		RequestLevelSwitch(currentLvl);
	}

	// Switch rooms if requested (after everything has finished computing, hence the end of the Render() function)
	if (!pleaseSwitchLevelsToThisOne.empty())
	{
		glClear(GL_COLOR_BUFFER_BIT);
		SwitchLevelAndSetUpLevelForPlayer(pleaseSwitchLevelsToThisOne);
		pleaseSwitchLevelsToThisOne.clear();
	}
}





bool TestRoom::SwitchLevelIO(std::string name)
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

	// Remember what level you're on!
	currentLvl = name;
	currentLvlFilename = levelFilename;		// TODO: This way we can parse and get the code from the filename!

    // Free loaded image
    stbi_image_free(imgData);
	return true;
}

void TestRoom::RequestLevelSwitch(std::string name)
{
	pleaseSwitchLevelsToThisOne = name;
}









void TestRoom::SwitchLevelAndSetUpLevelForPlayer(std::string name)
{
	// Clear things up first!!!
	Destruct();


	// TEST CODE to load a level
	bool success;
	success = SwitchLevelIO(name);
	//success = SwitchLevelIO("test");

	if (!success)
	{
		printf("ERROR: Room was not created, because level switching did not succeed\n");
		return;
	}

	for (int i = 0; i < gameObjects.size(); i++)
	{
		// Look for the player!!!
		if (dynamic_cast<TestGameObj*>(gameObjects.at(i)) != NULL)
		{
			camFocusObj = gameObjects.at(i);
			break;
		}
	}
}

void TestRoom::Destruct()
{
	// Tear down all the objects in the object list
	for (int it = 0; it != gameObjects.size(); ++it)
	{
		delete gameObjects.at(it);
	}
	gameObjects.clear();
}













std::string TestRoom::FindLevelIO(std::string name, std::string dir)         // The level has to be a png file in the levels folder
{
    // Search using dirent.h??? (what is that, anyways??)
    DIR* dirPoint = opendir(dir.c_str());
    dirent* entry = readdir(dirPoint);

    std::string searchCriteria = std::string("n_") + std::string(name.c_str()) + std::string(".png");

    while (entry)
    {
        // Only searches the top level
        if (entry->d_type == DT_REG)
        {
            // REGULAR FILE!!! Check if it matches
            std::string fname = entry->d_name;
            if (fname.find(searchCriteria, (fname.length() - searchCriteria.length())) != std::string::npos)
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

