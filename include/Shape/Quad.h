#pragma once

class Quad
{
    public:
        Quad(float width, float height);
//        Quad(float width, float height, Image);           TODO: make a textured quad constructor
        virtual ~Quad();

        void Render(float x, float y);

        // Getters and Setters
        void SetWidth(float width) { width = _width; }
        float GetWidth() { return _width; }

        void SetHeight(float height) { height = _height; }
        float GetHeight() { return _height; }

        // void SetImage(Image image) { image = _image; }
        // float GetImage() { return _image; }


    protected:

    private:
        float _width;
        float _height;
//        Image* _image;
};
