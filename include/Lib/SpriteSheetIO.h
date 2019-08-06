#pragma once

#if defined(__unix__) || defined(__APPLE__)
#include "json/json.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../json/json.h"
#endif

class Texture;

#include <string>

class SpriteSheetIO
{
public:
	SpriteSheetIO(Json::Value props);
	~SpriteSheetIO();

	void Render(std::string action, float x, float y, float w, float h);

private:
	Json::Value _properties;
	Texture* mySpriteSheet;
	std::string previousAction;
	int ticksOnAction;
};