#if defined(__unix__) || defined(__APPLE__)
#include "TileSet.h"
#include <SDL2/SDL_opengl.h>
#include "defs.h"
#include "Lib/stb_image.h"
#include "Lib/Texture.h"
#elif defined(_WIN32) || defined(WIN32)
#include "..\..\include\Rooms\TileSet.h"
#include <SDL_opengl.h>
#include "../../include/defs.h"
#include "../../include/Lib/stb_image.h"
#include "../../include/Lib/Texture.h"
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





// HELPER FUNCTION!!!
#define TS_GRID_WIDTH_HEIGHT 16
bool IsInrangeOfGrounds(int r_value, bool includeSlopes)
{

	bool checkRegGnd = ((r_value >= 1 && r_value <= 3) ||
		(r_value >= 16 && r_value <= 19) ||
		(r_value >= 33 && r_value <= 35) ||
		r_value == 51 ||
		(r_value >= 64 && r_value <= 67) ||
		(r_value >= 80 && r_value <= 82) ||
		(r_value >= 96 && r_value <= 98));

	if (checkRegGnd)
	{
		return true;
	}
	else if (includeSlopes)
	{
		return ((r_value >= 4 && r_value <= 11) ||
			(r_value >= 20 && r_value <= 27) ||
			(r_value >= 36 && r_value <= 41) ||
			(r_value >= 52 && r_value <= 57) ||
			(r_value >= 68 && r_value <= 71) ||
			(r_value >= 84 && r_value <= 87) ||
			(r_value >= 100 && r_value <= 101) ||
			(r_value >= 116 && r_value <= 117));
	}

	return false;
}

void AddQuadForTile(int r_value, int gx, int gy, std::vector<TileSet_Vector>& rendVectors)
{
    // Calculate all the things needed!!!
	float x = (float)gx * GRID_SIZE;
	float y = (float)gy * GRID_SIZE;



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




void TileSet::InterpretAndAddVector(int index, int gx, int gy, const int rmGWidth, const int rmGHeight, unsigned char* imgData)
{
    int r_value = (int)imgData[index * 3];

    // If 0 it's blank eh (or out of bounds)
	if (r_value <= 0 ||
		r_value >= TS_GRID_WIDTH_HEIGHT * TS_GRID_WIDTH_HEIGHT)
	{
		return;
	}




#pragma region Calc Ground Values

    // Do this test against 'the ground', since
    // the flat ground is hard to coordinate together
    // (Slopes are fine, so only do the flat ground!)
    // IT WILL BECOME AUTOMAGIC!!!! YES!!!
    bool ul_isInsideCorner = false;
	bool ur_isInsideCorner = false;				// These.... will be used later, when overlays need to be made.
    bool dl_isInsideCorner = false;
    bool dr_isInsideCorner = false;
    if (IsInrangeOfGrounds(r_value, false))
    {
        if (r_value == 18)
        {
            // This is the 'black block', or center or filler
            //... ignore for now!!
        }
        else
        {
            // So, here, you wanna calc which block it really should be from
            // the surrounding blocks!!!
            // ... and then, you'll just plug it into the interpreter.

            // Check for collision in surrounding 8
        	//	***
        	//	* *
			//	***
            bool isEmpties[8];
            for (int i = 0; i < 8; i++)
            {
            	{
            		// Have it skip #4 (bc that's me!!!)
            		int filteredInd = i > 3 ? i + 1 : i;

            		// Change into the correct x and y coords
            		int xoff = filteredInd % 3 - 1 + gx;
            		int yoff = filteredInd / 3 - 1 + gy;


            		// Check if offset vals aren't exiting the room
            		if (xoff < 0 || xoff >= rmGWidth ||
            			yoff < 0 || yoff >= rmGHeight)
            		{
                        // Set it as a wall so
                        // that maybe it'll load correctly in the
                        // filter l8r down the algorithm
                     	isEmpties[i] = false;
            		}
            		else
            		{
            			// Try testing it! (this time make sure that slopes are included, since in this case, they
            			// are ground we want to meld with)
						int foundRVal = (int)imgData[(yoff * rmGWidth + xoff) * 3];
            			isEmpties[i] = !IsInrangeOfGrounds(foundRVal, true);

            			// We want to know for this next part if is empty of ground!
            		}
            	}
            }


        	// THIS IS THE TESTING ZONE!!!
        	// Desc: The test will see if an inside-corner
        	// is needed, or if a block is a wall or a floor
        	// based off of the corners and walls it finds
        	//
        	// And then from all of that, it will assign a new
        	// r_value!
        	bool ul_isCorner = false;
        	bool ur_isCorner = false;
        	bool dl_isCorner = false;
        	bool dr_isCorner = false;
        	bool n_isWall = false, e_isWall = false, s_isWall = false, w_isWall = false;


        	// Start w/ the upper_left corner
        	//	**0
        	//	* 0
        	//	000
        	if (isEmpties[1] && isEmpties[3])
        	{
        		ul_isCorner = true;
        		n_isWall = true;
        		w_isWall = true;
        	}
        	else if (isEmpties[1])		// Just the upper wall
        	{
        		n_isWall = true;
        	}
        	else if (isEmpties[3])		// Just the left wall
        	{
        		w_isWall = true;
        	}
        	else if (isEmpties[0])		// Okay, if just the corner's empty, then it's an inside-corner!
        	{
        		ul_isInsideCorner = true;
        	}


        	// Yoshi! Tugi wa the upper_right corner
        	//	0**
        	//	0 *
        	//	000
        	if (isEmpties[1] && isEmpties[4])
        	{
        		ur_isCorner = true;
        		n_isWall = true;
        		e_isWall = true;
        	}
        	else if (isEmpties[1])		// Just the upper wall
        	{
        		n_isWall = true;
        	}
        	else if (isEmpties[4])		// Just the right wall
        	{
        		e_isWall = true;
        	}
        	else if (isEmpties[2])		// Okay, if just the corner's empty, then it's an inside-corner!
        	{
        		ur_isInsideCorner = true;
        	}





        	// Tugi wa the lower_left corner
        	//	000
        	//	* 0
        	//	**0
        	if (isEmpties[3] && isEmpties[6])
        	{
        		dl_isCorner = true;
        		s_isWall = true;
        		w_isWall = true;
        	}
        	else if (isEmpties[6])		// Just the lower wall
        	{
        		s_isWall = true;
        	}
        	else if (isEmpties[3])		// Just the left wall
        	{
        		w_isWall = true;
        	}
        	else if (isEmpties[5])		// Okay, if just the corner's empty, then it's an inside-corner!
        	{
        		dl_isInsideCorner = true;
        	}




        	// Saigo ni the lower_left corner wo kakunin siken no kaisi wo itasimasu
        	//	000
        	//	0 *
        	//	0**
        	if (isEmpties[4] && isEmpties[6])
        	{
        		dr_isCorner = true;
        		s_isWall = true;
        		e_isWall = true;
        	}
        	else if (isEmpties[6])		// Just the lower wall
        	{
        		s_isWall = true;
        	}
        	else if (isEmpties[4])		// Just the right wall
        	{
        		e_isWall = true;
        	}
        	else if (isEmpties[7])		// Okay, if just the corner's empty, then it's an inside-corner!
        	{
        		dr_isInsideCorner = true;
        	}




        	// OKAY, now take all of these collision
        	// discoveries and make some magic happen!
        	//
        	// ASSIGN THE NEW R_VALUE!!!!

        	if (ul_isCorner)		// Start w/ the upper left corner eh
        	{
        		if (dr_isCorner)	// Is it a 4-cornerer?????
        		{
        			// Sweet! Set the r_val!
        			r_value = 51;
        		}
        		else if (ur_isCorner)	// Is it a top of pillar????
        		{
        			// Cool!
        			r_value = 64;
        		}
        		else if (dl_isCorner)	// Is it a left-end-of-thin-platform????
        		{
        			// Heeeey
        			r_value = 65;
        		}
        		else
        		{
        			// Well, it is a regular corner, but still cool, i'd say!
        			r_value = 1;
        		}
        	}
        	else if (ur_isCorner)	// Start w/ the upper right corner....
        	{
        		if (dr_isCorner)	// Is it a right-end-of-thin-platform????
        		{
        			// Yes! I do say!
        			r_value = 67;
        		}
        		else
        		{
        			// Reg. corner guys
        			r_value = 3;
        		}
        	}
        	else if (dl_isCorner)	// Start w/ the lower left corner.....
        	{
        		if (dr_isCorner)	// Is it a bottom of a pillar????
        		{
        			// Saved my b8con!!!
        			r_value = 96;
        		}
        		else
        		{
        			// Regular corner
        			r_value = 33;
        		}
        	}
        	else if (dr_isCorner)	// Only 1 possibility left dude...
        	{
        		// A reg-reg!
        		r_value = 35;
        	}

        	// Okay, so from here on out, there will only be walls
        	// (or segments of pillars)
        	else if (n_isWall)		// Start with the north wall...
        	{
        		if (s_isWall)		// Could this be part of a hor. pillar???
        		{
        			// Hey hey hey!!!
        			r_value = 66;
        		}
        		else
        		{
        			// Normal wall
        			r_value = 2;
        		}
        	}
        	else if (e_isWall)		// Start w/ the east wall!
        	{
        		if (w_isWall)		// Am I.... the.... pillar section?????
        		{
        			// Woohooo!
        			r_value = 80;
        		}
        		else
        		{
        			// Nope, just a regular wall!
        			// But that's okay, I'm still important in this level!
        			r_value = 19;
        		}
        	}
        	else if (s_isWall)		// Well, nothing special from here on out, I guess...
        	{
        		// Reg.
        		r_value = 34;
        	}
        	else if (w_isWall)
        	{
        		// Reg.
        		r_value = 17;
        	}
        	else
        	{
        		// Wait wait wait... so you're not a corner,
        		// neither a wall?!?!?!?!
        		//
        		// Oh, I get it... the INSIDE CORNER on all 4
        		// sides, eh.
        		// Okay, I'll just give you the blank (not black)
        		// space so they can overlay it l8r, 'kay? Thanks!
        		r_value = 16;
        	}
        }
    }

#pragma endregion

    // I'm glad you could skip that whole testing
    // process if you're not flat-ground!





    // Load it on there!!!
	AddQuadForTile(r_value, gx, gy, rendVectors);



	// WAIT!
	// If there's a tile-overlay, create that too! (could be multiple eh)
	// (NOTE:: only applicable to flat-ground who went thru the test!)
	if (ul_isInsideCorner) { AddQuadForTile(98, gx, gy, rendVectors); }
	if (ur_isInsideCorner) { AddQuadForTile(97, gx, gy, rendVectors); }
	if (dl_isInsideCorner) { AddQuadForTile(82, gx, gy, rendVectors); }
	if (dr_isInsideCorner) { AddQuadForTile(81, gx, gy, rendVectors); }
}

// Take 1 texture and render parts of it all over the screen!!!!!
void TileSet::RenderVerts()
{
	if (rendVectors.size() <= 0)
	{
		// printf("ERROR: there are no tilesets loaded!\n");
		return;
	}

	// Enable textures
	glEnable(GL_TEXTURE_2D);
	myTileset->Bind(0);			// TODO make the 0 into actually can animate later on!!!! (goto texture.h eh)

	// Render quads (start from lower
	// left hand corner and go counter-
	// clockwise)
	glBegin(GL_QUADS);

	#if defined(__unix__) || defined(__APPLE__)
	for (int i = 0; i < rendVectors.size(); i++)
	{
        TileSet_Vector tsv = rendVectors.at(i);
	#elif defined(_WIN32) || defined(WIN32)
	for each (TileSet_Vector tsv in rendVectors)
	{
	#endif // __unix__

		glTexCoord2f(tsv.s, tsv.t);
		glVertex2f(tsv.x, tsv.y);
	}

	glEnd();


	// Undo textures
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, NULL);
}
