#pragma once

#include "Entity.h"

class SpriteSheetIO;

#include <string>


class Checkpoint : public Entity
{
    public:
        Checkpoint(int gx, int gy, TestRoom* rm);
        virtual ~Checkpoint();

        void Update();
        void Render();

        bool IsColliding(BoundBox* box);

        void YouLose(Entity* accordingToMe);

    protected:

    private:
        SpriteSheetIO* sprSheet;
};
