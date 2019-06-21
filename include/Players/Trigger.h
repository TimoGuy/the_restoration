#pragma once
#ifdef __unix__
#include <Object.h>
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Object.h"
#endif

#include <string>

class Room;

class Trigger : public Object
{
    public:
        Trigger(int gx, int gy, TestRoom* rm);
        virtual ~Trigger();

		void Update();
		void Render();

		void SetEntranceCoords(int gx, int gy);     // This is an optional function, which allows the player to appear anywhere in the room!
        bool GetCustomCoords(int& gx, int& gy);     // If set to custom, then it will automatically make this yeah!

        Base* GetNextEvent();       // This will return either a GameLevel or a Cutscene or a textbox, etc.

		bool IsColliding(BoundBox* box);

		bool IsDesiringToTrigger();

		void SetEventIDAndSetMaster(std::string nextEventID, bool isJustTouchToTrigger);      // This sets up the master and then goes thru and finds all slaves (neighboring triggers)
		std::string GetNewEventID();

		void SetMasterTriggerAndSetSlave(Trigger* masterTrig);

		bool NeedsSetup();          // Checks if either a master (has event id) or if a slave (has a master)

		void SetColliding(bool flag);
		bool IsColliding();

		void DisableMe();
		bool isDisabled();

    protected:

    private:
        bool active = true;

        void SetupSlaveRecur(int gx, int gy);       // Workings: 1: am I a slave?!?! then pass it on



        Trigger* _masterTrigger = NULL;        // If slave, will have ptr to master, if master, NULL

		bool _isJustTouchToTrigger;
		std::string _nextEventID;       // Include the 'c_' or 'n_' tags too please!

		int _width, _height;		// The reason why these are int's is bc we don't need floats! They're supposed to be a part of the grid!
		bool _isUpPressed;			// Just makes code look butter eh!
		bool _prevHadUpPressed;		// You want to keep track of this, that way you don't get ppl holding up to get into the door!

		// Only the master trigger would mess w/ this!
		bool isColliding = false;

		// For custom entrance positions
        int ceGX, ceGY;
};
