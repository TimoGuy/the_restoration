#ifdef __unix__
#include "ObjectFactory.h"

#include "TestGameObj.h"
#include "Ground.h"
#include "Exit.h"
#include "Slant.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../include/ObjectFactory.h"

#include "../include/Players/TestGameObj.h"
#include "../include/Players/Ground.h"
#include "../include/Players/Exit.h"
#include "../include/Players/Slant.h"
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
    evSlant,

    evEnd
};
static std::map<std::string, StringValue>* s_mapStringValues = NULL;







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
        (*s_mapStringValues)["160,160,160"] = evSlant;
        (*s_mapStringValues)["end"] = evEnd;

        printf("Init Object map!!\n");
    }

    static ObjectFactory factory;
    return factory;
}




Object* ObjectFactory::Build(std::string const& key, int gx, int gy, Room* rm) const
{
    // I apologize... this is all hard-coded in!
    switch ((*s_mapStringValues)[key.c_str()])
    {
    case evNotDefined:
        // This is white... so just a blank spot!
        break;

    case evPlayer:       // The Player!!!
        return new TestGameObj(gx, gy, rm);
        break;

    case evGround:           // Ground
        return new Ground(gx, gy, rm);
        break;

    case evExit:        // Exits (doors)
        return new Exit(gx, gy, rm);
        break;

    case evSlant:     // Slants (try to pass something in to calculate the slant eh!
        return new Slant(gx, gy, rm);
        break;

    default:
        printf("The color \"%s\" is invalid inside of \'ObjectFactory.cpp\'\'s list...\n", key.c_str());
        break;
    }

    return NULL;
}
