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

struct _Vector4
{
	float r, g, b, a;
};

SpriteSheetIO::SpriteSheetIO(Json::Value props)
{
	_properties = props;

	// Load up the sprite sheet
	printf(_properties["sprites"]["sprite_sheet"]["image"].asString().c_str());
	mySpriteSheet = new Texture(
		_properties["sprites"]["sprite_sheet"]["image"].asString(),
		STBI_rgb_alpha
	);

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

	// Call the real render function eh
	Render(action, x, y, w, h, ticksOnAction);
}

void SpriteSheetIO::Render(std::string action, float x, float y, float w, float h, int tickTime)
{
	// Input the offsets!
	x += _properties["sprites"]["sprite_sheet"]["render_off_x"].asFloat();
	y += _properties["sprites"]["sprite_sheet"]["render_off_y"].asFloat();

	// Calc which frame
	int adjustedTick =
        tickTime / _properties["sprites"][action]["ticks_per_frame"].asInt();

    int frame;
    if (_properties["sprites"][action]["repeat"].asBool())
    	frame = adjustedTick % _properties["sprites"][action]["images"].size();
    else
    	frame = adjustedTick >= _properties["sprites"][action]["images"].size()
    					? _properties["sprites"][action]["images"].size() - 1 : adjustedTick;

	// Anyways, render this sucker!
	Json::Value texCoords = _properties["sprites"][action]["images"][std::to_string(frame)];		// TODO: For now, 0, and then put in the animation once you know this works!
	float texW = _properties["sprites"]["sprite_sheet"]["width"].asFloat();
	float texH = _properties["sprites"]["sprite_sheet"]["height"].asFloat();

	float tx = texCoords["x"].asFloat() / texW;
	float ty = texCoords["y"].asFloat() / texH;
	float tx2 = texCoords["x2"].asFloat() / texW;
	float ty2 = texCoords["y2"].asFloat() / texH;

	// Calculate the color eh
	if (_properties["sprites"][action].isMember("colors"))
	{
		_Vector4 baseColor, nextColor;

		// Find the basecolor and nextcolor TODODODODODOD


		// Interpolate, or no?
		if (_properties["sprites"][action]["colors"]["interpolate"].asBool())
		{
			// Calc the interpolation eh

		}
		else
		{
			// Then it's just the base color
			glColor4f(baseColor.r, baseColor.g, baseColor.b, baseColor.a);
		}
	}

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
