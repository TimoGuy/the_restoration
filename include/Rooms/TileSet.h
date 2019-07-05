#pragma once

#ifdef __unix__
#include "Lib/Texture.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../Lib/Texture.h"
#endif

#include <vector>

// This will run thru and render all of
// these so as to make an efficient run
// of the tilesets (and later TODO make
// them be vbo's)
struct TileSet_Vector
{
	float x, y, z;		// For the positioning of vertices
	float s, t;			// For texture positioning
};

// Unfortunately, you need to do the interpreting
// yourself from the image file of the map of the tilesets eh
class TileSet
{
public:
	TileSet();
	~TileSet();

	void LoadTileTex(std::string tileSetFName);			// Please provide 'tileSetFName' as an absolute path!!!!
	void InterpretAndAddVector(int r_value, int gx, int gy);
	void RenderVerts();

private:
	std::vector<TileSet_Vector> rendVectors;	// The vectors to draw!!!!
	Texture* myTileset;			// Gets loaded on init
};

