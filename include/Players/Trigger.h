#pragma once
#ifdef __unix__
#include <Object.h>
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Object.h"
#endif

#include <string>

class Trigger : public Object
{
    public:
        Trigger(int gx, int gy, bool isJustTouchToTrigger, std::string nextRoomID, TestRoom* rm);
        virtual ~Trigger();

		void Update();
		void Render();

		void SetEntranceCoords(int gx, int gy);     // This is an optional function, which allows the player to appear anywhere in the room!
        bool GetCustomCoords(int& gx, int& gy);     // If set to custom, then it will automatically make this yeah!


		bool IsColliding(BoundBox* box);

		bool IsDesiringToTrigger();

		std::string GetNewRoomID();

    protected:

    private:
		bool _isJustTouchToTrigger;
		std::string _nextRoomID;

		int _width, _height;		// The reason why these are int's is bc we don't need floats! They're supposed to be a part of the grid!
		bool _isUpPressed;			// Just makes code look butter eh!
		bool _prevHadUpPressed;		// You want to keep track of this, that way you don't get ppl holding up to get into the door!


		// For custom entrance positions
        bool customEnter = false;
        int ceGX, ceGY;


		// TODO: ONLY FOR TESTIGN
		bool isColliding = false;
};
