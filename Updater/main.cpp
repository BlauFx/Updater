#include <Windows.h>
#include <Psapi.h>
#include <string>
#include <filesystem>
#include <iostream>

#define _tprintf wprintf

std::string ProcessName = "";
std::string ProcessName2 = "";

std::string utf8_encode(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

void GetProcessNameByID(DWORD processID, bool Change)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
        PROCESS_VM_READ,
        FALSE, processID);

    std::wstring string_string;

    if (NULL != hProcess)
    {
        TCHAR Buffer[MAX_PATH];

        if (GetModuleFileNameEx(hProcess, 0, Buffer, MAX_PATH))
            _tprintf(TEXT("%s  (PID: %u)\n"), Buffer, processID);

       std::wstring string_string(Buffer);
       ProcessName = utf8_encode(string_string);

       if (Change) {
           ProcessName2 = utf8_encode(string_string);
       }

       CloseHandle(hProcess);
    }
}

int main(int argc, char* argv[]) {

    if (argc <= 1) {
        exit(0);
    }

    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);

    std::string PID = argv[1];

    GetProcessNameByID(std::stoi(PID), true);

    bool IsProcessRunning = true;

    do {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, std::stoi(PID));

        if (hProcess != NULL) {
            CloseHandle(hProcess);
            GetProcessNameByID(std::stoi(PID), false);

            if (!(ProcessName._Equal(ProcessName2))) {
                IsProcessRunning = false;
            }
        }
        else {
            IsProcessRunning = false;
        }

        Sleep(500);

    } while (IsProcessRunning);


    std::string File_Location = ProcessName;
    std::size_t found = File_Location.find_last_of("/\\");

    File_Location = File_Location.substr(0, found);

    namespace fs = std::filesystem;
    for (fs::path p : fs::directory_iterator(File_Location)) {
        if (p.filename().compare("temp") == 0 || p.filename().compare("Songs") == 0) {
            continue;
        }

        std::string File_Location2 = File_Location + "\\temp\\old";
        fs::rename(p, File_Location2 / p.filename());
    }

    std::string File_Location2 = File_Location + "\\temp\\files";
    for (fs::path p : fs::directory_iterator(File_Location2)) {
        if (p.filename().compare("temp") == 0 || p.filename().compare("Songs") == 0) {
            continue;
        }

        fs::rename(p, File_Location / p.filename());
    }

    std::string Start = "start \"\" \"" + ProcessName2;
    Start += "\"";

    system(Start.c_str());
    
    return 0;
}
