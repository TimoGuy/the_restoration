#pragma once

// So there needs to be a TRIGGER that launches something!
// And this TRIGGER will take a REACTION object, which can
// do what the EXIT class does... change the room, or something
// like that! And then it'd be versitile and be able to launch a
// CUTSCENE object too!

// So basically, what's needed is to change the structure of the program a little bit...

// So think the OOT randomizer games,,, there's a trigger that runs something.

// Room is so that the main gameloop can run it.
// Reaction is so that you do a run() on this thing, and plug it into a TRIGGER.


#ifdef __unix__
#include "Rooms/Room.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../Room.h"
#endif

class CutsceneSprite;
class GameLoop;


#include <string>
#include <vector>

class CutsceneObject;

class Cutscene : public Room//, public Reaction
{
    public:
        Cutscene(std::string name, GameLoop* gloop);
        virtual ~Cutscene();

        void Update();

		void Render();
        void Render(bool resetViewMat);		// Use this function to select whether or not the glm should be reset

        void Delete(CutsceneObject* thisObj);
        void End(Room* nextRoom);
        CutsceneSprite* GetSpriteByID(int id);

    protected:

    private:
        int ticks;
        std::vector<CutsceneSprite*> sprites;
        std::vector<CutsceneObject*> objects;
};
