#pragma once

#include <jsoncpp/json/json.h>
#include <jsoncpp/json/writer.h>
#include <vector>
#include <string>
#include <fstream>

#define SERIAL_FILE_LOCATION ".data/data.json"


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

    Json::Value& GetJsonData() { return jsonData; }

    void SaveData()
    {
        std::ofstream save_file;
        save_file.open(SERIAL_FILE_LOCATION);

        Json::StyledWriter styledWriter;
        save_file << styledWriter.write(jsonData);
        save_file.close();
    }

private:
    SerialManager()
    {
        // Check if json file exists and read it in
        std::ifstream ifs(SERIAL_FILE_LOCATION);
        Json::Reader reader;
        Json::Value obj;
        reader.parse(ifs, jsonData);
    }

    Json::Value jsonData;
};

