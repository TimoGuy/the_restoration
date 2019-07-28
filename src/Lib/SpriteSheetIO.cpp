#if defined(__unix__) || defined(__APPLE__)
#include "Lib/SpriteSheetIO.h"
#include "Lib/Texture.h"
#include <SDL2/SDL_opengl.h>
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Lib/SpriteSheetIO.h"
#include "../../include/Lib/Texture.h"
#include <SDL_opengl.h>
#endif

#include <iostream>

SpriteSheetIO::SpriteSheetIO(Json::Value props)
{
	_properties = props;

	// Load up the sprite sheet
	mySpriteSheet = new Texture(
		_properties["sprites"]["sprite_sheet"]["image"].asString(),
		STBI_rgb_alpha);

	previousAction = "";
	ticksOnAction = 0;
}

SpriteSheetIO::~SpriteSheetIO()
{
	delete mySpriteSheet;
}

void SpriteSheetIO::Render(std::string action, float x, float y, float w, float h)
{
	// Check if same action as previous time
	if (action == previousAction)
	{
		ticksOnAction++;
	}
	else
	{
		// New action set, eh!
		ticksOnAction = 0;
		previousAction = action;
	}



	// Anyways, render this sucker!
	Json::Value texCoords = _properties["sprites"][action]["images"]["0"];		// TODO: For now, 0, and then put in the animation once you know this works!
	std::cout << texCoords << "\n";
	float tx = texCoords["x"].asFloat() / _properties["sprites"]["sprite_sheet"]["width"].asFloat();
	float ty = texCoords["y"].asFloat() / _properties["sprites"]["sprite_sheet"]["height"].asFloat();
	float tx2 = texCoords["x2"].asFloat() / _properties["sprites"]["sprite_sheet"]["width"].asFloat();
	float ty2 = texCoords["y2"].asFloat() / _properties["sprites"]["sprite_sheet"]["height"].asFloat();

	// Enable textures
    glEnable(GL_TEXTURE_2D);
    mySpriteSheet->Bind(0);

    // Render quad
    glBegin(GL_QUADS);

    glTexCoord2f(tx, ty);
    glVertex2f(x, y);

    glTexCoord2f(tx2, ty);
    glVertex2f(x + w, y);

    glTexCoord2f(tx2, ty2);
    glVertex2f(x + w, y + h);

    glTexCoord2f(tx, ty2);
    glVertex2f(x, y + h);

    glEnd();



    // Undo textures
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, NULL);
}
