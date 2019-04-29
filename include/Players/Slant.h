#ifdef __unix__
#include <Object.h>
#elif defined(_WIN32) || defined(WIN32)
#include "../Object.h"
#endif

class Slant : public Object
{
    public:
        Slant(int gx, int gy, Room* rm);
        virtual ~Slant();

        void Update() {}
        void Render() {}

		bool IsColliding(BoundBox* box) { return false; }
    protected:

    private:
};
