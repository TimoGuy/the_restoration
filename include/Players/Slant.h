#include <Object.h>


class Slant : public Object
{
    public:
        Slant(int gx, int gy, Room* rm);
        virtual ~Slant();

        void Update() {}
        void Render() {}

        bool IsColliding(BoundBox* box) { }
    protected:

    private:
};
