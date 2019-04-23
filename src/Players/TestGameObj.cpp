#include "TestGameObj.h"
#include <stdio.h>
#include "InputManager.h"


TestGameObj::TestGameObj()
{
    // Make image
    image = new Quad(32, 48);
}

TestGameObj::~TestGameObj()
{
    //dtor
}

void TestGameObj::Update()
{
	// Adjust according to input
	x += InputManager::Instance().x();
	y += InputManager::Instance().y();
	printf("%f     %f\n", x, y);
}

void TestGameObj::Render()
{
	image->Render(x, y);
}
