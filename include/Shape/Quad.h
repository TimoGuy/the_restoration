#pragma once
#ifdef __unix__
#include "Lib/Texture.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../Lib/Texture.h"
#endif

class Quad
{
    public:
        Quad(float width, float height);
        Quad(float width, float height, Texture* tex);
//        Quad(float width, float height, Image);           TODO: make a textured quad constructor
        virtual ~Quad();

        void Render(float x, float y);

        // Getters and Setters
        void SetWidth(float width) { width = _width; }
        float GetWidth() { return _width; }

        void SetHeight(float height) { height = _height; }
        float GetHeight() { return _height; }

        void SetImage(Texture* image) { image = _image; }
        Texture* GetImage() { return _image; }


    protected:

    private:
        float _width;
        float _height;
        Texture* _image = NULL;
};
