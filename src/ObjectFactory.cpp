#ifdef __unix__
#include "ObjectFactory.h"

#include "TestGameObj.h"
#include "Ground.h"
#include "Exit.h"
#include "Slant.h"
#include "Hazard.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../include/ObjectFactory.h"

#include "../include/Players/Ground.h"
#include "../include/Players/Exit.h"
#include "../include/Players/Slant.h"
#include "../include/Players/Hazard.h"
#include "../include/Players/TopSideCollGround.h"

#include "../include/Players/Entity.h"
#include "../include/Players/TestGameObj.h"
#include "../include/Players/TestEnemy.h"
#endif

#include <stdio.h>
#include <map>

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

    evEnd
};
static std::map<std::string, StringValue>* s_mapStringValues = NULL;

int ObjectFactory::previousStringValue = -1;
Object* ObjectFactory::previousSlant = nullptr;


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
        (*s_mapStringValues)["end"] = evEnd;

        printf("Init Object map!!\n");
    }

    static ObjectFactory factory;
    return factory;
}




Object* ObjectFactory::Build(std::string const& key, int gx, int gy, Room* rm) const
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
		retObj = new Exit(gx, gy, false, std::string("fitt"), rm);		// TODO: FIRST: we will just load the level with no params... l9r we'll add those.
        break;

	case evSlantRight:
		if (previousStringValue != evSlantRight)
		{
			previousSlant = new Slant(gx, gy, rm, false);
			retObj = previousSlant;
		}
		else
		{
			// Extend the current slant!
			((Slant*)previousSlant)->Extend1BlockToTheRight(gx, gy, rm);
		}
		break;

	case evSlantLeft:
		if (previousStringValue != evSlantLeft)
		{
			previousSlant = new Slant(gx, gy, rm, true);
			retObj = previousSlant;
		}
		else
		{
			// Extend the current slant!
			((Slant*)previousSlant)->Extend1BlockToTheRight(gx, gy, rm);
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

    default:
        printf("The color \"%s\" is invalid inside of \'ObjectFactory.cpp\'\'s list...\n", key.c_str());
        break;
    }

	// Update the previous key
	previousStringValue = (*s_mapStringValues)[key.c_str()];

	// Spit it out, BOBBUSS!!
    return retObj;
}
