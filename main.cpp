#include <stdio.h>
#include <stdlib.h>

#if defined(__unix__) || defined(__APPLE__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>
#include "GameLoop.h"
#include "InputManager.h"
#include "defs.h"

#include <unistd.h>
#define GetCurrentDir getcwd

#elif defined(_WIN32) || defined(WIN32)
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>
#include "include\GameLoop.h"
#include "include\InputManager.h"
#include "include\defs.h"

#include <direct.h>
#define GetCurrentDir _getcwd
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

    if (TTF_Init() < 0)
    {
        printf("TTF could not initialize! TTF_Error: %s\n", TTF_GetError());
        return false;
    }



    // Create window!
    window = SDL_CreateWindow(
        TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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

    TTF_Quit();

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

    char buff[FILENAME_MAX];
    GetCurrentDir(buff, FILENAME_MAX);
    printf("Program running in:\n\t%s\n", buff);

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
