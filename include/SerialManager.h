#pragma once

#ifdef __unix__
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/writer.h>
#elif defined(_WIN32) || defined(WIN32)
// TODO: Set up JSON w/ windows!
#endif

#include <vector>
#include <string>
#include <fstream>

#define SERIAL_FILE_LOCATION ".data/data.json"


// Just gonna put the default values for the game_vars right here...
#define GAME_VAR_DEF_saw_tutorial false
#define GAME_VAR_DEF_player_max_jumps 1


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




    void SetGameData_Bool(char* key, bool flag)
    {
        Json::Value& vars = GetJsonData();
        vars["game_vars"][key] = flag;
        SaveData();
    }
    bool GetGameData_Bool(char* key, bool defaultVal)
    {
        Json::Value& vars = GetJsonData();
        if (!vars["game_vars"].isMember(key))
            SetGameData_Bool(key, defaultVal);       // Set and save the default value

        // Pull the value now!
        return vars["game_vars"][key].asBool();
    }




    void SetGameData_Int(char* key, int flag)
    {
        Json::Value& vars = GetJsonData();
        vars["game_vars"][key] = flag;
        SaveData();
    }
    int GetGameData_Int(char* key, int defaultVal)
    {
        Json::Value& vars = GetJsonData();
        if (!vars["game_vars"].isMember(key))
            SetGameData_Int(key, defaultVal);       // Set and save the default value

        // Pull the value now!
        return vars["game_vars"][key].asInt();
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

