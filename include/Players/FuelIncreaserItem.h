#pragma once
#include "Entity.h"
#include <string>

class FuelIncreaserItem : public Entity
{
    public:
        FuelIncreaserItem(int gx, int gy, TestRoom* rm);
        virtual ~FuelIncreaserItem();


        void Update();
        void Render();

        bool IsColliding(BoundBox* box);

        void YouLose(Entity* accordingToMe);

    protected:

    private:
        std::string serial;
        bool active;
};
