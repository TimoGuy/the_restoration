#if defined(__unix__) || defined(__APPLE__)
#include "TopSideCollGround.h"
#include "defs.h"
#include "Lib/Texture.h"
#include "Quad.h"
#elif defined(_WIN32) || defined(WIN32)
#include "..\..\include\Players\TopSideCollGround.h"
#include "../../include/defs.h"
#include "../../include/Lib/Texture.h"
#include "../../include/Shape/Quad.h"
#endif



TopSideCollGround::TopSideCollGround(int gx, int gy, TestRoom* rm) : Ground(gx, gy, rm)
{
	// Override image! even though it's already been loaded....
	delete image;
	image = new Quad(GRID_SIZE, GRID_SIZE, new Texture(std::string(".data/textures/one_sided_block_test.png"), STBI_rgb_alpha));
}


TopSideCollGround::~TopSideCollGround()
{
}

bool TopSideCollGround::IsColliding(BoundBox* box)
{
	// Check if previous mvt. showed it was above the box!
	if (box->prevy + box->height > y)
	{
		return false;
	}

	// Use 'image' as the bounding box and test collision!
	return x < box->x + box->width &&
		x + GRID_SIZE > box->x &&
		y < box->y + box->height &&
		y + GRID_SIZE > box->y;
}
