#pragma once

#include <vector>

class Texture;
struct _LifeAnimationBeam;

class LifeBar
{
    public:
        LifeBar();
        virtual ~LifeBar();

        void Render(int currentLife, float x, float y);
    protected:
    private:
        int prevLife;       // To track if lost life!!!

        Texture* spriteSheetHearts;
        Texture* texBeam;

        std::vector<_LifeAnimationBeam> beams;
};
