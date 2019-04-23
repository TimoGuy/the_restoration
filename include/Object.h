#pragma once

class Object
{
    public:
        Object();
        virtual ~Object();

        virtual void Update() = 0;
        virtual void Render() = 0;

    protected:

    private:
};
