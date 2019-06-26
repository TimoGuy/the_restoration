#ifdef __unix__
#include "ObjectFactory.h"

#include "Ground.h"
#include "Trigger.h"
#include "Slant.h"
#include "Hazard.h"
#include "TopSideCollGround.h"
#include "MovingPlatGround.h"

#include "Entity.h"
#include "TestGameObj.h"
#include "TestEnemy.h"
<<<<<<< Updated upstream
=======
#include "Npc.h"


>>>>>>> Stashed changes
#elif defined(_WIN32) || defined(WIN32)
#include "../include/ObjectFactory.h"

#include "../include/Players/Ground.h"
#include "../include/Players/Trigger.h"
#include "../include/Players/Slant.h"
#include "../include/Players/Hazard.h"
#include "../include/Players/TopSideCollGround.h"
#include "../include/Players/MovingPlatGround.h"

#include "../include/Players/Entity.h"
#include "../include/Players/TestGameObj.h"
#include "../include/Players/TestEnemy.h"
#include "../include/Players/Npc.h"
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
    evTrigger,

	evSlantRight,
	evSlantLeft,

    evHazard,

	evTopSideCollGround,

	evEnemy1,

	evMovingPlatGround,

	evNpc,

    evEnd
};
static std::map<std::string, StringValue>* s_mapStringValues = NULL;







// Constructor
ObjectFactory::ObjectFactory()
{
    // Initialize!!!
    s_mapStringValues = new std::map<std::string, StringValue>();

    (*s_mapStringValues)["255,255,255"] = evNotDefined;
    (*s_mapStringValues)["255,216,0"] = evPlayer;
    (*s_mapStringValues)["0,0,0"] = evGround;
    (*s_mapStringValues)["38,127,0"] = evTrigger;
    (*s_mapStringValues)["0,162,232"] = evSlantRight;
    (*s_mapStringValues)["153,217,234"] = evSlantLeft;
    (*s_mapStringValues)["255,0,0"] = evHazard;
    (*s_mapStringValues)["100,100,100"] = evTopSideCollGround;
    (*s_mapStringValues)["255,174,201"] = evEnemy1;
	(*s_mapStringValues)["136,0,21"] = evMovingPlatGround;
	(*s_mapStringValues)["239,228,176"] = evNpc;
    (*s_mapStringValues)["end"] = evEnd;

    printf("Init Object map!!\n");

    // Default vals
    previousStringValue = -1;
    previousObj = NULL;
}


// Singleton initialization
ObjectFactory& ObjectFactory::GetObjectFactory()
{
    static ObjectFactory factory;
    return factory;
}


Object* ObjectFactory::Build(std::string const& key, int gx, int gy, TestRoom* rm)
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






    case evTrigger:        // Triggers (usu. exits or doors)
        // Create the trigger object
		retObj = new Trigger(gx, gy, rm);
        break;




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

	case evNpc:
		retObj = new Npc(gx, gy, rm);
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
