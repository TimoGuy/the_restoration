#pragma once

#include <vector>

class Texture;
struct _LifeAnimationBeam;
class SpriteSheetIO;

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
        SpriteSheetIO* beamSheet;

        std::vector<_LifeAnimationBeam> beams;
};
