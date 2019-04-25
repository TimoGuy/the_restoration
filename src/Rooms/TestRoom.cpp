#include "TestRoom.h"
#include "Players/TestGameObj.h"
#include "Lib/Texture.h"
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include "ObjectFactory.h"
#include <iterator>
#include <sstream>
#include <SDL2/SDL_opengl.h>
#include <algorithm>

TestRoom::TestRoom()
{
    // Set up
//    player = new TestGameObj();






    // TEST CODE to load a level
//    Texture level(std::string(".data/testlvl.png"), STBI_rgb);




    SwitchLevelIO("jojo");


    for (int i = 0; i < gameObjects.size(); i++)
    {
        // Look for the player!!!
        if (dynamic_cast<TestGameObj*>(gameObjects.at(i)) != NULL)
        {
            camFocusObj = gameObjects.at(i);
            break;
        }
    }

    // Make the collision map!





}

TestRoom::~TestRoom()
{
    // Tear down all the objects in the object list
    for (int it = 0; it != gameObjects.size(); ++it)
    {
        delete gameObjects.at(it);
    }
    gameObjects.clear();
}

void TestRoom::Update()
{
    for (int it = 0; it != gameObjects.size(); ++it)
    {
        gameObjects.at(it)->Update();
    }
//    player->Update();









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
        camX = std::max(0, std::min((int)camX + SCREEN_WIDTH, gWidth * GRID_SIZE));
    }
    if (!lockY)
    {
        camY = std::max(0, std::min((int)camY, gHeight * GRID_SIZE - SCREEN_HEIGHT));
    }
}

void TestRoom::Render()
{
    // Set the camera
    glTranslatef(-camX, -camY, 0.0f);

    // Pry a render for everyone!
    for (int it = 0; it != gameObjects.size(); ++it)
    {
        gameObjects.at(it)->Render();
    }
//    player->Render();

    // Undo the camera
    glTranslatef(camX, camY, 0.0f);
}





void TestRoom::SwitchLevelIO(std::string name)
{
    // Find/Search for the requested level (within the 'n_...' form-factor)
    std::string currentDir = std::string(get_current_dir_name()) + std::string("/.data/levels/");
    printf("Levels are stored in:\n%s\n\n", currentDir.c_str());
    std::string levelName = TestRoom::FindLevelIO(name, currentDir);
//    if (levelName.)           // TODO implement the error checking someway....
//    {
//        printf("ERROR: Unable to switch levels... no file was found with the level name %s\n", name);
//        return;
//    }

    // Load a level
    int comp;
//    width, height;        // These are member variables now
    const std::string& fileName = currentDir + levelName;
    int req = STBI_rgb;
    unsigned char* imgData = stbi_load(fileName.c_str(), &gWidth, &gHeight, &comp, req);

    if (imgData == NULL)
    {
        printf("Error! Texture loading failed for \"%s\"\n", fileName.c_str());
        return;
    }

    // Initialize the Collision Map
    collisionMap = new Object*[gWidth * gHeight];
    for (int uu = 0; uu < gWidth * gHeight; uu++)
    {
//        printf("%i\n", collisionMap[uu]);
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
//        out_string = ss.str();


        std::string colorId =
            ss[0].str() + std::string(",") +
            ss[1].str() + std::string(",") +
            ss[2].str();

        Object* _new = ObjectFactory::GetObjectFactory().Build(colorId.c_str(), (int)(i % gWidth), (int)(i / gWidth), this);
        if (_new != NULL)
            gameObjects.push_back(_new);        // Adds the returned built object!

        i++;
    }




    // Print out the values of the image
    /*std::ofstream myfile;
    myfile.open ("example.txt");

    int i = 0;
    while (i < width * height)
    {
//        printf("%i, %i, %i\n", imgData[(i * 3) + 0], imgData[(i * 3) + 1], imgData[(i * 3) + 2]);
        if (req == STBI_rgb)
        {
//            printf("rgb\n");
            myfile << (int)imgData[(i * 3) + 0] << ",  \t" << (int)imgData[(i * 3) + 1] << ",  \t" << (int)imgData[(i * 3) + 2] << "\n";
        }
        else if (req == STBI_rgb_alpha)
        {
//            printf("rgba\n");
            myfile << (int)imgData[(i * 4) + 0] << ",  \t" << (int)imgData[(i * 4) + 1] << ",  \t" << (int)imgData[(i * 4) + 2] << ",  \t" << (int)imgData[(i * 4) + 3] << "\n";
        }
        i++;
    }


    myfile.close();*/


    // Free loaded image
    stbi_image_free(imgData);
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



    return NULL;
}

