#pragma once

#if defined(__unix__) || defined(__APPLE__)
#elif defined(_WIN32) || defined(WIN32)
#endif

class Texture;


class Quad
{
    public:
        Quad(float width, float height);
        Quad(float width, float height, Texture* tex);
        virtual ~Quad();

        void Render(float x, float y);

        // Getters and Setters
        void SetWidth(float width) { _width = width; }
        float GetWidth() { return _width; }

        void SetHeight(float height) { _height = height; }
        float GetHeight() { return _height; }

        void SetImage(Texture* image) { _image = image; }
        Texture* GetImage() { return _image; }


    protected:

    private:
        float _width;
        float _height;
        Texture* _image = nullptr;
};
