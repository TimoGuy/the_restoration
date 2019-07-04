#ifdef __unix__
#include "TileSet.h"
#include <SDL2/SDL_opengl.h>
#include "defs.h"
#elif defined(_WIN32) || defined(WIN32)
#include "..\..\include\Rooms\TileSet.h"
#include <SDL_opengl.h>
#include "../../include/defs.h"
#endif


TileSet::TileSet()
{
}

TileSet::~TileSet()
{
	delete myTileset;
}




void TileSet::LoadTileTex(std::string tileSetFName)
{
	myTileset = new Texture(tileSetFName, STBI_rgb_alpha);		// It should be alpha, since we'll be pulling out of it, but it dosn't matter too much perhaps!
	printf("Tileset loaded successfully!\n");
}

void TileSet::InterpretAndAddVector(int r_value, int gx, int gy)
{
#define TS_GRID_WIDTH_HEIGHT 16

	// If 0 it's blank eh (or out of bounds)
	if (r_value <= 0 ||
		r_value >= TS_GRID_WIDTH_HEIGHT * TS_GRID_WIDTH_HEIGHT)
	{
		return;
	}

	// Calculate all the things needed!!!
	float x = gx * GRID_SIZE;
	float y = gy * GRID_SIZE;



	// Calc texture stuff (on a 16x16 grid eh)
	float oneGridSize = 1.0f / (float)TS_GRID_WIDTH_HEIGHT;
	float s = (r_value % TS_GRID_WIDTH_HEIGHT) * oneGridSize;
	float t = (r_value / TS_GRID_WIDTH_HEIGHT) * oneGridSize;


	// Generate the vectors!
	// (start from lower left hand corner and go counter-clockwise)
	TileSet_Vector newTsv = {
		x, y, 0,
		s, t
	};
	rendVectors.push_back(newTsv);

	newTsv = {
		x + GRID_SIZE, y, 0,
		s + oneGridSize, t
	};
	rendVectors.push_back(newTsv);

	newTsv = {
		x + GRID_SIZE, y + GRID_SIZE, 0,
		s + oneGridSize, t + oneGridSize
	};
	rendVectors.push_back(newTsv);

	newTsv = {
		x, y + GRID_SIZE, 0,
		s, t + oneGridSize
	};
	rendVectors.push_back(newTsv);
}

// Take 1 texture and render parts of it all over the screen!!!!!
void TileSet::RenderVerts()
{
	if (rendVectors.size() <= 0)
	{
		printf("ERROR: there are no tilesets loaded!\n");
		return;
	}

	// Enable textures
	glEnable(GL_TEXTURE_2D);
	myTileset->Bind(0);			// TODO make the 0 into actually can animate later on!!!! (goto texture.h eh)

	// Render quads (start from lower
	// left hand corner and go counter-
	// clockwise)
	glBegin(GL_QUADS);

	for each (TileSet_Vector tsv in rendVectors)
	{
		glTexCoord2f(tsv.s, tsv.t);
		glVertex2f(tsv.x, tsv.y);
	}

	glEnd();


	// Undo textures
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, NULL);
}
