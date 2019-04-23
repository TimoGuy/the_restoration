#include "TestRoom.h"
#include "Players/TestGameObj.h"
#include <stdio.h>

TestRoom::TestRoom()
{
    // Set up
    player = new TestGameObj();
}

TestRoom::~TestRoom()
{
    // Tear
    delete player;
}

void TestRoom::Update()
{
    player->Update();
}

void TestRoom::Render()
{
    player->Render();
}

