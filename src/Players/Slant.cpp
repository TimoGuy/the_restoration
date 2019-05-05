#ifdef __unix__
#include "Slant.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Players/Slant.h"
#include "../../include/defs.h"
#endif

#include <stdio.h>




Slant::Slant(int gx, int gy, Room* rm, bool isLeft) : Object(gx, gy, rm, true)
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
	rm->getCollisionMap()[rm->getGWidth() * gy + gx] = this;
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

	if (isCool)
	{
		glColor3f(0, 1, 0);
		isCool = false;
	}
	else
	{
		glColor3f(1, 1, 1);
	}

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
	float m = (_end.y - _start.y) / (_end.x - _start.x) * -1.0f;	// It turns out negative for some reason, so let's edit that!
	//float b = _start.y - m * _start.x;		// Now we have the y=mx+b formula!!! And don't have to calc the sine cosine stuff.


											// Calc any top or bottom collision
	if (m > 0)				// Upwards
	{
		float y = (box.max.x - _start.x) * m /*+ b*/ + _start.y;
		// TEST
		//printf("y=%f\nm=%f\n"/*b=%f\n*/"box.max.x=%f\nbox.max.y=%f\n\n\n\n", y, m, /*b,*/ box.max.x, box.max.y);

		if (y < box.max.y &&
			y > box.min.y)
		{
			printf("Hey ho!\n");
			isCool = true;
			return true;
		}
	}
	else if (m < 0)			// Downwards
	{
		// Get left side Checked
		float y = box.min.x * m/* + b*/;

		if (y < box.max.y &&
			y > box.min.y)
			return true;
	}
	else /*if (m == 0)*/	// Straight
	{
		/*if (b < box.max.y &&
			b > box.min.y)*/
			return true;
	}

	return false;
}




void Slant::Extend1BlockToTheRight(int newGx, int newGy, Room* rm)
{
	if (_blocks >= SLANT_PROPERTIES_MAX_BLOCKS ||
		originalGy != newGy)
		// Not gonna happen
		return;



	// Move the right point to the right more
	_blocks++;
	_end.x += GRID_SIZE;

	// Add myself to the new spots I take up too!!!
	rm->getCollisionMap()[rm->getGWidth() * newGy + newGx] = this;
}
