#pragma once

#include <vector>
#include <string>


/// It's a singleton for serializing objects for saving and such.
/// Right now there is no save functionality, however objects can
/// check if they collide with any serializations on creation!
class SerialManager
{
public:
    static SerialManager& Instance()
    {
        static SerialManager* _instance = new SerialManager();
        return *_instance;
    }

    std::vector<std::string>& GetSerials() { return serials; }

    bool CheckIfSerialIsTaken(std::string serial)
    {
        for (unsigned int i = 0; i < serials.size(); i++)
        {
            if (serial == serials.at(i))
            {
                // It's taken!
                return true;
            }
        }

        // Not taken eh
        return false;
    }

private:
    SerialManager()
    {

    }

    std::vector<std::string> serials;
};

