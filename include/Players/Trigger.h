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
        Trigger(int gx, int gy, bool isJustTouchToTrigger, std::string nextEventID, TestRoom* rm);
        virtual ~Trigger();

		void Update();
		void Render();

		void SetEntranceCoords(int gx, int gy);     // This is an optional function, which allows the player to appear anywhere in the room!
        bool GetCustomCoords(int& gx, int& gy);     // If set to custom, then it will automatically make this yeah!

        Room* GetNextEvent();       // This will return either a GameLevel or a Cutscene, etc.

		bool IsColliding(BoundBox* box);

		bool IsDesiringToTrigger();

		std::string GetNewEventID();

    protected:

    private:
		bool _isJustTouchToTrigger;
		std::string _nextEventID;       // Include the 'c_' or 'n_' tags too please!

		int _width, _height;		// The reason why these are int's is bc we don't need floats! They're supposed to be a part of the grid!
		bool _isUpPressed;			// Just makes code look butter eh!
		bool _prevHadUpPressed;		// You want to keep track of this, that way you don't get ppl holding up to get into the door!


		// For custom entrance positions
        int ceGX, ceGY;


		// TODO: ONLY FOR TESTIGN
		bool isColliding = false;
};
