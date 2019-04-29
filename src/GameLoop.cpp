#ifdef __unix__
#include "GameLoop.h"
#include "Rooms/TestRoom.h"
#include <SDL2/SDL_opengl.h>
#include "InputManager.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../include/GameLoop.h"
#include "../include/Rooms/TestRoom.h"
#include <SDL_opengl.h>
#include "../include/InputManager.h"
#endif

GameLoop::GameLoop(SDL_Window* window)
{
    _window = window;
}

bool GameLoop::Execute()
{
    // This will be the Room obj
    Room* testRoom = new TestRoom();

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
        testRoom->Update();





		// Render
		glClear(GL_COLOR_BUFFER_BIT);

        // Setup proj mat
//        glUniformMatrix4fv(resources::shaderStandard->GetLocProjectMat(), 1, false, (GLfloat*)&resources::projectionMat[0]);
//        resources::modelViewMat = glm::mat4();
        testRoom->Render();



        SDL_GL_SwapWindow(_window);



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
    }

    return true;
}

GameLoop::~GameLoop()
{
    // Thanks for letting me use it!
    _window = NULL;
}
