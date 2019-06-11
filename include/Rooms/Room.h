#pragma once

#ifdef __unix__
//#include "Object.h"
#elif defined(_WIN32) || defined(WIN32)
//#include "../Object.h"
//#include "../Players/Entity.h"
#endif

class Room
{
    public:
        Room();
        virtual ~Room();

        virtual void Update() = 0;
        virtual void Render() = 0;

    protected:

    private:
};
