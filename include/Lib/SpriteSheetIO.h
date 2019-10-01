#pragma once

#if defined(__unix__) || defined(__APPLE__)
#include "json/json.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../json/json.h"
#endif

class Texture;
class GameLoop;

#include <string>

class SpriteSheetIO
{
public:
	SpriteSheetIO(Json::Value props, GameLoop* gloop=NULL);
	~SpriteSheetIO();

	void ForceAnimationFrameReset();

	void Render(std::string action, float x, float y, float w, float h, float speedMod=1.0f);
	void Render(std::string action, float x, float y, float w, float h, float speedMod, int tickTime);

private:
	void CalculateCoordsFromGridValsIfNeeded(std::string action);

	Json::Value _properties;
	Texture* mySpriteSheet;
	std::string previousAction;
	GameLoop* _gloop;
	float ticksOnAction;
};
