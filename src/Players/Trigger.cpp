#ifdef __unix__
#include "Trigger.h"
#include "TestRoom.h"
#include "Cutscene.h"
#include "InputManager.h"
#include "defs.h"
#include <SDL2/SDL_opengl.h>
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Players/Trigger.h"
#include "../../include/InputManager.h"
#include "../../include/defs.h"
#include <SDL_opengl.h>
#endif

#include <stdio.h>

Trigger::Trigger(int gx, int gy, bool isJustTouchToTrigger, std::string nextEventID, TestRoom* rm) : Object(gx, gy, rm, false)
{
    //ctor
	printf("Trigger built!\tIt leads to the event \"%s\"\n", nextEventID.c_str());
	_isJustTouchToTrigger = isJustTouchToTrigger;
	_nextEventID = nextEventID;

	// Set default dimensions eh
	_width = _height = GRID_SIZE;

	// Add to collision map
	rm->getCollisionMap()[rm->getGWidth() * gy + gx] = this;

	_isUpPressed = false;
	_prevHadUpPressed = false;

	// Default val is -1 anyways
	ceGX = ceGY = -1;
}

Trigger::~Trigger()
{
    //dtor
}



void Trigger::Update()
{
	// Pass previous input b4 reading new input stream
	_prevHadUpPressed = _isUpPressed;
	_isUpPressed = InputManager::Instance().y() < 0;		// Neg. on the y axis means upwards!
}

void Trigger::Render()
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




void Trigger::SetEntranceCoords(int gx, int gy)
{
    ceGX = gx;
    ceGY = gy;

    // That's it, I suppose...
    // We'll have to change the entrance code for the player eh...
}



bool Trigger::GetCustomCoords(int& gx, int& gy)        // This'll edit the variables, warning you!!! (it's a dipstick function)
{
    // Now insert the gx,gy etc.
    gx = ceGX;
    gy = ceGY;

    return ceGX >= 0 || ceGY >= 0;
}



Room* Trigger::GetNextEvent()
{
    // Check what type of event
    switch (GetNewEventID()[0])
    {
    case 'c':
        return new Cutscene(GetNewEventID(), room->GetGameLoop());
        break;

    case 'n':
        return new TestRoom(GetNewEventID(), room->GetGameLoop(), ceGX, ceGY);
        break;
    }
}



bool Trigger::IsColliding(BoundBox* box)
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





bool Trigger::IsDesiringToTrigger()
{
	return _isJustTouchToTrigger ||
		(!_prevHadUpPressed && _isUpPressed);
}

std::string Trigger::GetNewEventID()
{
	return _nextEventID;
}
