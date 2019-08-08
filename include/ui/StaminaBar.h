#pragma once

class Texture;


class StaminaBar
{
    public:
        StaminaBar();
        virtual ~StaminaBar();

        void Render(int currentStaminas, float x, float y);
    protected:
    private:
        // For rendering
        float currentStaminas_asFloat;      // This is to animate the object
        Texture* spriteSheetBorder;
        Texture* spriteSheetFilling;
};
