#ifdef __unix__
#include "Slant.h"
#include "defs.h"
#include "TestRoom.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Players/Slant.h"
#include "../../include/defs.h"
#endif

#include <stdio.h>
#include <algorithm>



Slant::Slant(int gx, int gy, TestRoom* rm, bool isLeft) : Object(gx, gy, rm, true)
{
    //ctor
    printf("Slant built!\n");
	originalGy = gy;		// If when extending this isn't correct, then we know it's wrapped around
	_isLeft = isLeft;
	_blocks = 1;

	// Build the points!
	_start.x = x;
	_end.x = x + GRID_SIZE;
	if (_isLeft)
	{
		_start.y = y;				// High
		_end.y = y + GRID_SIZE;		// Low
	}
	else
	{
		_start.y = y + GRID_SIZE;
		_end.y = y;
	}

	// Add to collision map
	((TestRoom*)rm)->getCollisionMap()[((TestRoom*)rm)->getGWidth() * gy + gx] = this;
}

Slant::~Slant()
{
    //dtor
}


void Slant::Render()
{
	// Make sure these are off
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, NULL);

	glColor3f(1, 1, 1);			// Color of the ground (WHITE I know!!!)

	// Draw it!!!!
	glBegin(GL_LINES);

	glVertex2f(_start.x, _start.y);
	glVertex2f(_end.x, _end.y);

	glEnd();
}





// These are for defining the structure of the slant object
bool Slant::IsColliding(BoundBox* otherBox)
{
	Box box;
	box.max.x = otherBox->x + otherBox->width;
	box.max.y = otherBox->y + otherBox->height;
	box.min.x = otherBox->x;
	box.min.y = otherBox->y;

	// Calc slope and y-offset
	float m = (_end.y - _start.y) / (_end.x - _start.x) * -1.0f;	// Now we have the y=mx+b formula!!! And don't have to calc the sine cosine stuff.


	// First see if horizontally the object would collide
	if (box.min.x > _end.x ||
		box.max.x < _start.x)
		return false;


	// Calc any top or bottom collision
	if (m > 0)				// Upwards SLANT
	{
		float y = (std::min(box.max.x, _end.x) - _start.x) * -m + _start.y;		// Reverse m because y is flipped here!

		if (y < box.max.y &&
			y > box.min.y)
		{
			return true;
		}
	}
	else if (m < 0)			// Downwards
	{
		// Get left side Checked
		float y = (std::max(box.min.x, _start.x) - _start.x) * -m + _start.y;		// Reverse m because y is flipped here!

		if (y < box.max.y &&
			y > box.min.y)
		{
			return true;
		}
	}
	else /*if (m == 0)*/	// Straight
	{
		float y = 0;
		if (y < box.max.y &&
			y > box.min.y)
			return true;
	}

	return false;
}




void Slant::Extend1BlockToTheRight(int newGx, int newGy)
{
	// Because texturing problems, we'll limit number of length for a slant...
	if (_blocks >= SLANT_PROPERTIES_MAX_BLOCKS ||
		originalGy != newGy)
		// Not gonna happen
		return;

	printf("\tSlant was extended by 1!!\n");

	// Move the right point to the right more
	_blocks++;
	_end.x += GRID_SIZE;

	// Add myself to the new spots I take up too!!!
	((TestRoom*)room)->getCollisionMap()[((TestRoom*)room)->getGWidth() * newGy + newGx] = this;
}
