#pragma once

class Texture;

#include <string>

class Minimap
{
    public:
        Minimap(std::string picFname);
        virtual ~Minimap();

        void Render(float rendX, float rendY, int centGX, int centGY);
    protected:
    private:
        Texture* mapTexture;
};
