#ifdef __unix__
#include "Trigger.h"
#include "TestRoom.h"
#include "Textbox.h"
#include "Cutscene.h"
#include "InputManager.h"
#include "defs.h"
#include <SDL2/SDL_opengl.h>
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Players/Trigger.h"
#include "../../include/Rooms/TestRoom.h"
#include "../../include/Players/Textbox.h"
#include "../../include/Rooms/Cutscene/Cutscene.h"
#include "../../include/InputManager.h"
#include "../../include/defs.h"
#include <SDL_opengl.h>
#endif

#include <stdio.h>

Trigger::Trigger(int gx, int gy, TestRoom* rm) : Object(gx, gy, rm, false)
{
    _isJustTouchToTrigger = false;

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
    isColliding = false;		// Just reset just in case...

	// Pass previous input b4 reading new input stream
	_prevHadUpPressed = _isUpPressed;
	_isUpPressed = InputManager::Instance().y() < 0;		// Neg. on the y axis means upwards!
}

void Trigger::Render()
{
	if (!InputManager::Instance().b4())
		return;




	if (IsColliding())
	{
		// Ready to enter!!!
		glColor3f(0.15f, 0.5f, 0);
	}
	else
	{
		// Idle
		glColor3f(0.13f, 0.7f, 0.3f);
	}


	if (isDisabled())
	{
        glColor3f(0.5f, 0, 0);
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
	if (_masterTrigger == NULL)
	{
		ceGX = gx;
		ceGY = gy;
	}
	else
	{
		_masterTrigger->SetEntranceCoords(gx, gy);
	}

    // That's it, I suppose...
    // We'll have to change the entrance code for the player eh...
}



bool Trigger::GetCustomCoords(int& gx, int& gy)        // This'll edit the variables, warning you!!! (it's a dipstick function)
{
	if (_masterTrigger == NULL)
	{
		// Now insert the gx,gy etc.
		gx = ceGX;
		gy = ceGY;

		return ceGX >= 0 || ceGY >= 0;
	}
	else
	{
		return _masterTrigger->GetCustomCoords(gx, gy);
	}
}



Base* Trigger::GetNextEvent()
{
    // Check what type of event
    switch (GetNewEventID().at(0))
    {
    case 'c':
        return new Cutscene(GetNewEventID(), room->GetGameLoop());
        break;

    case 'n':
	{
		int tmpCeGX, tmpCeGY;
		Trigger::GetCustomCoords(tmpCeGX, tmpCeGY);
		return new TestRoom(GetNewEventID(), room->GetGameLoop(), tmpCeGX, tmpCeGY);
		break;
	}

    case 'm':
        return new Textbox(0, 0, GetNewEventID(), 28, std::function<void()>(), room);
        break;

    default:
        return NULL;
        break;
    }
}



bool Trigger::IsColliding(BoundBox* box)
{
	// Okay, time for the real collision check!
	bool col = x < box->x + box->width &&
		x + _width > box->x &&
		y < box->y + box->height &&
		y + _height > box->y;

    if (col)
    {
        // Report to master
        SetColliding(col);
    }

    // Return value FROM Master!!
    return IsColliding();
}





bool Trigger::IsDesiringToTrigger()
{
	if (isDisabled())
		return false;

	return _isJustTouchToTrigger ||
		(!_prevHadUpPressed && _isUpPressed);
}







void Trigger::SetEventIDAndSetMaster(std::string nextEventID, bool isJustTouchToTrigger)
{
	printf("Trigger-group set up!\tIt leads to the event \"%s\"\n", nextEventID.c_str());
	_isJustTouchToTrigger = isJustTouchToTrigger;
	_nextEventID = nextEventID;

	// So it's a master now
	_masterTrigger = NULL;      // Just in case

    // FIND ALL NEIGHBORING TRIGGERS!!!!
    // And set them up to be slaves!!!!!
    // Start w/ nearby
    int gx = (int)x / GRID_SIZE;
    int gy = (int)y / GRID_SIZE;
    SetupSlaveRecur(gx - 1, gy);
    SetupSlaveRecur(gx, gy + 1);
    SetupSlaveRecur(gx + 1, gy);
}

void Trigger::SetupSlaveRecur(int gx, int gy)
{
    // Check if in bounds
    if (gx >= 0 && gx < room->getGWidth() &&
        gy >= 0 && gy < room->getGHeight())
    {
        // Are you a Trigger.h???
        Object* tmp = room->getCollisionMap()[room->getGWidth() * gy + gx];
        if (dynamic_cast<Trigger*>(tmp) != NULL)
        {
            // Yay! Now let's see if you're set up
            Trigger* tmpTrigger = (Trigger*)tmp;

            if (tmpTrigger->NeedsSetup())
            {
                // Awesome!!!! NOW YOU'RE MY SLAVE
                tmpTrigger->SetMasterTriggerAndSetSlave(this);

                // So since this one was a good one,
                // LET'S RECURSE!!!!
                SetupSlaveRecur(gx - 1, gy);
                SetupSlaveRecur(gx, gy + 1);
                SetupSlaveRecur(gx + 1, gy);
            }
        }
    }
}

void Trigger::SetMasterTriggerAndSetSlave(Trigger* masterTrig)
{
    //printf("\tslave in trigger-group found!\n");
    _masterTrigger = masterTrig;
    _nextEventID = "";
    _isJustTouchToTrigger = masterTrig->_isJustTouchToTrigger;
}




std::string Trigger::GetNewEventID()
{
	if (_masterTrigger == NULL)
	{
		return _nextEventID;
	}
	else
	{
		return _masterTrigger->GetNewEventID();
	}
}




bool Trigger::NeedsSetup()
{
    if (_masterTrigger != NULL)
    {
        // It's a slave
        return false;
    }
    else if (!GetNewEventID().empty())
    {
        // It's a master
        return false;
    }

    // I guess if it's neither, then it needs set up
    return true;
}

void Trigger::SetColliding(bool flag)
{
    if (_masterTrigger == NULL)
    {
        isColliding = flag;
    }
    else
    {
        _masterTrigger->isColliding = flag;
    }
}

bool Trigger::IsColliding()
{
    if (_masterTrigger == NULL)
    {
        return isColliding;
    }
    else
    {
        return _masterTrigger->isColliding;
    }
}


void Trigger::DisableMe()
{
    if (_masterTrigger == NULL)
    {
        active = false;
    }
    else
    {
        _masterTrigger->active = false;
    }
}

bool Trigger::isDisabled()
{
    if (_masterTrigger == NULL)
    {
        return !active;
    }
    else
    {
        return !_masterTrigger->active;
    }
}
