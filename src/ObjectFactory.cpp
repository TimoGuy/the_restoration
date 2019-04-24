#include "ObjectFactory.h"

#include "TestGameObj.h"
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
        printf("Init Object map!!\n");
        (*s_mapStringValues)["255,216,0"] = evPlayer;
        (*s_mapStringValues)["0,0,0"] = evGround;
        (*s_mapStringValues)["38,127,0"] = evExit;
        (*s_mapStringValues)["160,160,160"] = evSlant;
        (*s_mapStringValues)["end"] = evEnd;
    }

    static ObjectFactory factory;
    return factory;
}


Object* ObjectFactory::Build(std::string const& key) const
{
    // I apologize... this is all hard-coded in!
    switch ((*s_mapStringValues)[key.c_str()])
    {
    case evPlayer:       // The Player!!!
        return new TestGameObj();
        break;

    case evGround:           // Ground
        break;

    case evExit:        // Exits (doors)
        break;

    case evSlant:     // Slants (try to pass something in to calculate the slant eh!
        break;

    default:
        printf("The color \"%s\" is invalid inside of \'ObjectFactory.cpp\'\'s list...\n", key);
        break;
    }

    return NULL;
}
