#include <stdio.h>
#include <stdlib.h>

#ifdef __unix__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "GameLoop.h"
#include "defs.h"
#elif defined(_WIN32) || defined(WIN32)
#include <SDL.h>
#include <SDL_opengl.h>
#include "include\GameLoop.h"
#include "include\defs.h"
#endif

SDL_Window* window;
SDL_GLContext glContext;

bool Setup()
{
    // Init SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }



    // Create window!
    window = SDL_CreateWindow(
        TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }


	// Setup OpenGL
    glContext = SDL_GL_CreateContext(window);
	if (glContext == NULL)
	{
		printf("GL context did not get created. Error: %s\n", SDL_GetError());
		return false;
	}






	//Set the viewport
    glViewport(0.0f,0.0f, SCREEN_WIDTH, SCREEN_HEIGHT);

    //Initialize Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0);

    //Initialize Modelview Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // GL properties
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.25f, 0.5f, 1.0f);

    return true;
}

void Destruct()
{
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    window = NULL;
}













////////////////////////////////////////////////
////////////////////////////////////////////////
/////////             MAIN           ///////////
/////////           FUNCTION         ///////////
////////////////////////////////////////////////
////////////////////////////////////////////////
int main(int argc, char** argv)
{
    printf("Hello World! I\'m back!\n");

    if (!Setup())
    {
        return 1;
    }

    // Run this main game loop!
    GameLoop mainLoop(window);
    if (!mainLoop.Execute())
    {
        return 1;
    }

    Destruct();

    return 0;
}
