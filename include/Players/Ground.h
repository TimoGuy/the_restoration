#pragma once
#include <Object.h>


class Ground : public Object
{
    public:
        Ground();
        virtual ~Ground();

        void Update() {}
        void Render() {}

    protected:

    private:
};
