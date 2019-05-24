#ifdef __unix__
#include "Exit.h"
#include "InputManager.h"
#include "defs.h"
#include <SDL2/SDL_opengl.h>
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Players/Exit.h"
#include "../../include/InputManager.h"
#include "../../include/defs.h"
#include <SDL_opengl.h>
#endif

#include <stdio.h>

Exit::Exit(int gx, int gy, bool isJustTouchToTrigger, std::string nextRoomID, Room* rm) : Object(gx, gy, rm, false)
{
    //ctor
	printf("Exit built!\tIt leads to \"*n_%s.png\"\n", nextRoomID.c_str());
	_isJustTouchToTrigger = isJustTouchToTrigger;
	_nextRoomID = nextRoomID;

	// Set default dimensions eh
	_width = _height = GRID_SIZE;

	// Add to collision map
	rm->getCollisionMap()[rm->getGWidth() * gy + gx] = this;

	_isUpPressed = false;
	_prevHadUpPressed = false;
}

Exit::~Exit()
{
    //dtor
}



void Exit::Update()
{
	// Pass previous input b4 reading new input stream
	_prevHadUpPressed = _isUpPressed;
	_isUpPressed = InputManager::Instance().y() < 0;		// Neg. on the y axis means upwards!
}

void Exit::Render()
{


	if (isColliding)
	{
		// Ready to enter!!!
		glColor3f(0.15f, 0.5f, 0);
		isColliding = false;		// Just reset just in case...
	}
	else
	{
		// Idle
		glColor3f(0.13f, 0.7f, 0.3f);
	}




	// Render a quad!!!!
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + _width, y);
	glVertex2f(x + _width, y + _height);
	glVertex2f(x, y + _height);
	glEnd();




	// Undo color... set to default white!!
	glColor3f(1, 1, 1);
}





bool Exit::IsColliding(BoundBox* box)
{
	// If it's colliding, check if it's the kind of exit
	// where by touching, you go, or you have to press up!
	/*if (!_isJustTouchToTrigger)
	{
		if (!_prevHadUpPressed &&
			_isUpPressed)
		{
			// You may continue, good sir!
		}
		else
		{
			// So it wasn't up... failed attempt!
			return false;
		}
	}*/

	// NOTE: shouldn't the player be handling this?? The only person USING
	// the exits is the player entity...


	// Okay, time for the real collision check!
	return isColliding = x < box->x + box->width &&
		x + _width > box->x &&
		y < box->y + box->height &&
		y + _height > box->y;
}





bool Exit::IsDesiringToTrigger()
{
	return _isJustTouchToTrigger ||
		(!_prevHadUpPressed && _isUpPressed);
}

std::string Exit::GetNewRoomID()
{
	return _nextRoomID;
}
