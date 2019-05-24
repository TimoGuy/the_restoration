#ifdef __unix__
#include "ObjectFactory.h"

#include "Ground.h"
#include "Exit.h"
#include "Slant.h"
#include "Hazard.h"
#include "TopSideCollGround.h"
#include "MovingPlatGround.h"

#include "Entity.h"
#include "TestGameObj.h"
#include "TestEnemy.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../include/ObjectFactory.h"

#include "../include/Players/Ground.h"
#include "../include/Players/Exit.h"
#include "../include/Players/Slant.h"
#include "../include/Players/Hazard.h"
#include "../include/Players/TopSideCollGround.h"
#include "../include/Players/MovingPlatGround.h"

#include "../include/Players/Entity.h"
#include "../include/Players/TestGameObj.h"
#include "../include/Players/TestEnemy.h"
#endif

#include <stdio.h>
#include <map>
#include <sstream>

// The Registered 'Objects'
enum StringValue
{
    evNotDefined,

    evPlayer,
    evGround,
    evExit,

	evSlantRight,
	evSlantLeft,

    evHazard,

	evTopSideCollGround,

	evEnemy1,

	evMovingPlatGround,

    evEnd
};
static std::map<std::string, StringValue>* s_mapStringValues = NULL;

int ObjectFactory::previousStringValue = -1;
Object* ObjectFactory::previousObj = NULL;


// Singleton initialization
ObjectFactory& ObjectFactory::GetObjectFactory()
{
    if (s_mapStringValues == NULL)
    {
        // Initialize!!!
        s_mapStringValues = new std::map<std::string, StringValue>();

        (*s_mapStringValues)["255,255,255"] = evNotDefined;
        (*s_mapStringValues)["255,216,0"] = evPlayer;
        (*s_mapStringValues)["0,0,0"] = evGround;
        (*s_mapStringValues)["38,127,0"] = evExit;
		(*s_mapStringValues)["0,162,232"] = evSlantRight;
		(*s_mapStringValues)["153,217,234"] = evSlantLeft;
		(*s_mapStringValues)["255,0,0"] = evHazard;
		(*s_mapStringValues)["100,100,100"] = evTopSideCollGround;
		(*s_mapStringValues)["255,174,201"] = evEnemy1;
		(*s_mapStringValues)["136,0,21"] = evMovingPlatGround;
        (*s_mapStringValues)["end"] = evEnd;

        printf("Init Object map!!\n");
    }

    static ObjectFactory factory;
    return factory;
}



//                                                       ↆEvery time you use a param, delete it!
Object* ObjectFactory::Build(std::string const& key, std::vector<std::string>* rmParams, int gx, int gy, Room* rm) const
{
    // I apologize... this is all hard-coded in!
	Object* retObj = NULL;
    switch ((*s_mapStringValues)[key.c_str()])
    {
    case evNotDefined:
        // This is white... so just a blank spot!
		retObj = NULL;
		break;

    case evPlayer:       // The Player!!!
		retObj = new TestGameObj(gx, gy, rm);
        break;

    case evGround:           // Ground
		retObj = new Ground(gx, gy, rm);
        break;






    case evExit:        // Exits (doors)
    {
        // Get the first 'e' code from the params
        int pos = -1;
        for (int i = 0; i < rmParams->size(); i++)
        {
            if (rmParams->at(i) == std::string("e"))
            {
                // Start here and just start reading the string thru to get values!
                pos = i;
                break;
            }
        }

        if (pos < 0)
        {
            // Break out and print error
            printf("\n\nERROR:: Not enough \'e\' params in level to create an exit...\n\n\n");
            break;
        }

        // Create the exit object
        bool touchTrigger;
        std::istringstream(rmParams->at(pos + 1)) >> touchTrigger;
		retObj = new Exit(gx, gy, touchTrigger, rmParams->at(pos + 2), rm);

        int end = 3;
		if (rmParams->at(pos + 3) != std::string("e") &&
            rmParams->at(pos + 3) != std::string("n"))
        {
            end = 4;    // A fourth param...

            // Parse from the x value (i.e. has a 123x456 to display coords)
            int ceGX, ceGY;

            std::string token;
            std::istringstream tokenStream(rmParams->at(pos + 3));
            int times = 0;
            while (std::getline(tokenStream, token, 'x') &&
                times <= 1)
            {
                if (times == 0)     // X axis
                {
                    std::istringstream(token) >> ceGX;
                }
                else if (times == 1)    // Y axis
                {
                    std::istringstream(token) >> ceGY;
                }

                times++;
            }

            printf("\n\n\tCustom entrance for %i,%i\n\n\n", ceGX, ceGY);
            ((Exit*)retObj)->SetEntranceCoords(ceGX, ceGY);
        }
        else
        {
            printf("\tNo custom exit code found\n");
        }

        // Remove those values
        rmParams->erase(rmParams->begin() + pos, rmParams->begin() + pos + end);

        break;
    }




	case evSlantRight:
		if (previousStringValue != evSlantRight)
		{
			previousObj = new Slant(gx, gy, rm, false);
			retObj = previousObj;
		}
		else
		{
			// Extend the current slant!
			((Slant*)previousObj)->Extend1BlockToTheRight(gx, gy);
		}
		break;

	case evSlantLeft:
		if (previousStringValue != evSlantLeft)
		{
			previousObj = new Slant(gx, gy, rm, true);
			retObj = previousObj;
		}
		else
		{
			// Extend the current slant!
			((Slant*)previousObj)->Extend1BlockToTheRight(gx, gy);
		}
		break;

    case evHazard:
		retObj = new Hazard(gx, gy, rm);
        break;

	case evTopSideCollGround:
		retObj = new TopSideCollGround(gx, gy, rm);
		break;

	case evEnemy1:
		retObj = new TestEnemy(gx, gy, rm);
		break;

	case evMovingPlatGround:
		if (previousStringValue != evMovingPlatGround)
		{
			previousObj = new MovingPlatGround(gx, gy, rm);
			retObj = previousObj;
		}
		else
		{
			// Extend the current moving platform!
			((MovingPlatGround*)previousObj)->Extend1BlockToTheRight(gx, gy);
		}
		break;

    default:
        printf("The color \"%s\" is invalid inside of \'ObjectFactory.cpp\'\'s list...\n", key.c_str());
        break;
    }

	// Update the previous key
	previousStringValue = (*s_mapStringValues)[key.c_str()];

	// Spit it out, BOBBUSS!!
    return retObj;
}
