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

class Cutscene : public Room, public Reaction
{
    public:
        Cutscene();
        virtual ~Cutscene();

    protected:

    private:
};
