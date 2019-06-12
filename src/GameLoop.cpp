#ifdef __unix__
#include "GameLoop.h"
#include "Rooms/TestRoom.h"
#include "Cutscene.h"
#include <SDL2/SDL_opengl.h>
#include "InputManager.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../include/GameLoop.h"
#include "../include/Rooms/TestRoom.h"
#include <SDL_opengl.h>
#include "../include/InputManager.h"
#endif


#include <iostream>


GameLoop::GameLoop(SDL_Window* window)
{
    _window = window;
}

bool GameLoop::Execute()
{
    // This will be the Room obj
    SetRoom(new Cutscene("c_test.txt", this));

    // This will hold up the thread...
    // I'll be honest, there's really only
    // supposted to be 1 main thread you know?
    // TODO: maybe make this a singleton or something????

	float msAllotted = 1000.f / 60.f;
	float targetTime = SDL_GetTicks();

    while (_running)
    {
        // Event system
        InputManager::Instance().ProcessInput(this);    // If the input desires, 'this' game loop can be terminated from this function!

		// Update
        GetRoom()->Update();





		// Render
		glClear(GL_COLOR_BUFFER_BIT);

        // Setup proj mat
//        glUniformMatrix4fv(resources::shaderStandard->GetLocProjectMat(), 1, false, (GLfloat*)&resources::projectionMat[0]);
//        resources::modelViewMat = glm::mat4();
        GetRoom()->Render();



        SDL_GL_SwapWindow(_window);






        // FOR DEBUG: check if the room needs to be reloaded
        if (InputManager::Instance().reloadRoom())		// This is a check, but after 1 check the inside variable turns off, so no worries.
        {
            // EDIT: Make it so that you can change the level to test using <iostream>
            std::cout << "Please enter the new level\'s name: ";
            std::string newLvl;
            std::getline(std::cin, newLvl);
            if (!newLvl.empty())
                // Load the level eh
                SetRoom(new TestRoom(newLvl.c_str(), this));
            else
                printf("ERROR: Level name was empty!!\n");
        }

        // FOR DEBUG: check if player wants to trigger a cutscene
        if (InputManager::Instance().reloadCutscene())		// This is a check, but after 1 check the inside variable turns off, so no worries.
        {
            // EDIT: Make it so that you can change the level to test using <iostream>
            std::cout << "Please enter the cutscene\'s name: ";
            std::string newCut;
            std::getline(std::cin, newCut);
            if (!newCut.empty())
                // Cutscene time!!!
                SetRoom(new Cutscene(newCut.c_str(), this));
            else
                printf("ERROR: Cutscene name was empty!!\n");
        }









		// Matouze
        if (SDL_GetTicks() >= (targetTime += msAllotted))
        {
            // Either right on or deficit (lagging)
            printf("This frame lagged!\n");
            targetTime = SDL_GetTicks();    // Get back on track!
        }
        else
        {
            // Delay for next frame
            SDL_Delay((unsigned int)targetTime - SDL_GetTicks());
        }

		//SDL_Delay(1000 / 30);		// Clamp to <30 fps for debugging
    }

    return true;
}



void GameLoop::SetRoom(Room* newRoom)
{
    // Remove current one
//    if (currentRoom != NULL)              // TODO:: there's problems w/ delete currentRoom!!!!!!
//        delete currentRoom;

    currentRoom = newRoom;        // This will shift to the currentRoom when the time is right ;)
}
Room* GameLoop::GetRoom() { return currentRoom; }


GameLoop::~GameLoop()
{
    // Thanks for letting me use it!
    _window = NULL;
}
