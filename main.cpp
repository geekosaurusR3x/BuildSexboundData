#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <windows.h>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

void BuildPlayerIdFile(std::vector<string>&);
void BuildEmptyPlayerIdFile();
bool CheckSuccess();
void CleanUpBuild();
bool CreateDirectoryStructure();
vector<string> DiscoverPlayerFiles();
void DumpPlayerData(std::vector<string>&);

int main()
{
    CreateDirectoryStructure();

    vector< string > playerFiles = DiscoverPlayerFiles();

    // If *.player files were found in the storage directory then process them.
    if (playerFiles.size() > 0) {
        DumpPlayerData(playerFiles);

        BuildPlayerIdFile(playerFiles);
    } else {
        BuildEmptyPlayerIdFile();
    }

    if (CheckSuccess()) {
        cout << "Success! Sexbound API Data was built successfully!" << endl;
    } else {
        cout << "Failed! Something went wrong!" << endl;
    }

    CleanUpBuild();

    cout << "\nPress any key to close this window. Enjoy! :)" << endl;

    cin.get();

    return 0;
}

void BuildEmptyPlayerIdFile() {
    std::ofstream o("SexboundAPI_Data\\playeridentities.json");

    o << "{}";
}

void BuildPlayerIdFile(std::vector<string>& playerFiles) {
    json jobject;

    for (int i = 0; i < playerFiles.size(); ++i) {
        json j;

        string filePath("SexboundAPI_Data\\build\\");
        filePath.append(playerFiles[i]);

        ifstream jsonFile(filePath);

        jsonFile >> j;

        string uuid = j["content"]["uuid"];

        jobject[ uuid.c_str() ] = j["content"]["identity"];

        jsonFile.close();
    }

    std::ofstream o("SexboundAPI_Data\\playeridentities.json");
    o << std::setw(4) << jobject << std::endl;
}

bool CheckSuccess() {
    const boost::filesystem::path FILE_PATH("SexboundAPI_Data\\playeridentities.json");

    return boost::filesystem::exists(FILE_PATH);
}

void CleanUpBuild() {
    const boost::filesystem::path dir("SexboundAPI_Data\\build");

    if (boost::filesystem::exists(dir)) {
        boost::filesystem::remove_all(dir);
    }
}

bool CreateDirectoryStructure() {
    const boost::filesystem::path dir("SexboundAPI_Data\\build");

	if( boost::filesystem::create_directories(dir) ) {
		cout << "Finished building directory structure!\n" << endl;

		return true;
	} else return false;
}

vector<string> DiscoverPlayerFiles() {
    const boost::filesystem::path dir("..\\storage\\player");

    const boost::regex my_filter( ".*\\.player" );

    vector< string > all_matching_files;

    boost::filesystem::directory_iterator end_itr; // Default ctor yields past-the-end
    for( boost::filesystem::directory_iterator i( dir ); i != end_itr; ++i )
    {
        // Skip if not a file
        if( !boost::filesystem::is_regular_file( i->status() ) ) continue;

        boost::smatch what;

        // Skip if no match for V2:
        //if( !boost::regex_match( i->leaf(), what, my_filter ) ) continue;
        // For V3:
        if( !boost::regex_match( i->path().filename().string(), what, my_filter ) ) continue;

        // File matches, store it
        all_matching_files.push_back( i->path().filename().string() );
    }

    return all_matching_files;
}

void DumpPlayerData(std::vector<string>& playerFiles) {
    for (int i = 0; i < playerFiles.size(); ++i) {
        string command = "..\\win32\\dump_versioned_json.exe ";
        string params  = "..\\storage\\player\\";
        params.append(playerFiles[i]);
        params.append(" SexboundAPI_Data\\build\\");
        params.append(playerFiles[i]);

        SHELLEXECUTEINFO ShExecInfo = {0};
        ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
        ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        ShExecInfo.hwnd = NULL;
        ShExecInfo.lpVerb = NULL;
        ShExecInfo.lpFile = command.c_str();
        ShExecInfo.lpParameters = params.c_str();
        ShExecInfo.lpDirectory = NULL;
        ShExecInfo.nShow = SW_HIDE;
        ShExecInfo.hInstApp = NULL;
        ShellExecuteEx(&ShExecInfo);
        WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
    }
}
