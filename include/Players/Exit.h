#pragma once
#ifdef __unix__
#include <Object.h>
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Object.h"
#endif

#include <string>

class Exit : public Object
{
    public:
        Exit(int gx, int gy, bool isJustTouchToTrigger, std::string nextRoomID, Room* rm);
        virtual ~Exit();

		void Update();
		void Render();

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

		// TODO: ONLY FOR TESTIGN
		bool isColliding = false;
};
