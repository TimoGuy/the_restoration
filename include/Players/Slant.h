#include <Object.h>


class Slant : public Object
{
    public:
        Slant(int gx, int gy);
        virtual ~Slant();

        void Update() {}
        void Render() {}

    protected:

    private:
};
