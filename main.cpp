#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

int main()
{
    string starboundDir ("\"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Starbound\"");
    string assetUnpacker ("..\\win32\\asset_unpacker.exe");

    json jobject;
    vector<string*> command;

    // Make 'SexboundAPI_Data' directory if it does not exist.
    command.push_back(new string("if not exist SexboundAPI_Data mkdir SexboundAPI_Data"));

    // Make 'build' directory.
    command.push_back(new string("if not exist SexboundAPI_Data\\build mkdir SexboundAPI_Data\\build"));

    // Delete temp file if it exists.
    command.push_back(new string("if exist SexboundAPI_Data\\build\\temp.dat del SexboundAPI_Data\\build\\temp.dat"));

    // Dump versioned JSON for each unique stored player file.
    command.push_back(new string("for %f in (..\\storage\\player\\*.player) do start ..\\win32\\dump_versioned_json.exe %f SexboundAPI_Data\\build\\%~nf.json"));

    // Write file names to temp file.
    command.push_back(new string("for %f in (..\\storage\\player\\*.player) do echo %~nf>> SexboundAPI_Data\\build\\temp.dat"));

    // Execute each command
    for (int i = 0; i < command.size(); ++i) {
        system(command[i]->c_str());

        delete command[i]; // Delete pointer
    }

    // Read in data from 'temp.dat' file.
    ifstream temp("SexboundAPI_Data\\build\\temp.dat");

    for( std::string line; getline( temp, line ); )
    {
        json j;

        string filePath("SexboundAPI_Data\\build\\");

        filePath.append(line);
        filePath.append(".json");

        ifstream jsonFile(filePath);

        jsonFile >> j;

        jobject[ line.c_str() ] = j["content"]["identity"];

        jsonFile.close();
    }

    temp.close();

    cout << jobject << endl;

    std::ofstream o("SexboundAPI_Data\\playeridentities.json");
    o << std::setw(4) << jobject << std::endl;

    return true;
}
