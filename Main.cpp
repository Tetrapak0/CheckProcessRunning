#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>
#include <chrono>
#include <string>
#include <cctype>
#include <algorithm>
#include <thread>

#pragma warning(disable : 4996)

using namespace std;

bool wasrunning;
string processName;
bool autoKill;
bool killed = false;
string title;

void setAutoKill()
{
    while (true)
    {
        if ((GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_F3) & 0x8000))
        {
            if (!autoKill)
            {
                autoKill = true;
                title = processName + " - " + "autokill = true" + " - CheckProcessRunning";
                SetConsoleTitleA(title.c_str());
            }
            else
            {
                autoKill = false;
                title = processName + " - " + "autokill = false" + " - CheckProcessRunning";
                SetConsoleTitleA(title.c_str());
            }
            Sleep(1000);
        }
    }
}

bool IsProcessRunning(const TCHAR* const executableName) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (!Process32First(snapshot, &entry)) {
        CloseHandle(snapshot);
        return false;
    }

    do {
        if (!_tcsicmp(entry.szExeFile, executableName)) {
            CloseHandle(snapshot);
            wasrunning = true;
            string endCommand = "taskkill /f /im \"" + processName + "\"";
            if (!killed)
            {
                if (autoKill)
                {
                    system(endCommand.c_str());
                    killed = true;
                }
                if (GetKeyState(VK_CONTROL) && GetKeyState(VK_END))
                {
                    system(endCommand.c_str());
                    killed = true;
                }
            }
            return true;
        }
    } while (Process32Next(snapshot, &entry));

    CloseHandle(snapshot);
    return false;
}

int main()
{
    SetConsoleTitleA("CheckProcessRunning");
    cout.setf(ios::boolalpha);
    HANDLE hInput;
    DWORD prev_mode;
    hInput = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hInput, &prev_mode);
    SetConsoleMode(hInput, prev_mode & ENABLE_EXTENDED_FLAGS | (prev_mode & ~ENABLE_QUICK_EDIT_MODE));
    cout << "WARNING!!!" << endl << "This application has highest level administrator privileges. End system processes at your own risk!" << endl << endl;
    system("pause");
    system("cls");
    cout << "WARNING!!!" << endl << "This application has highest level administrator privileges. End system processes at your own risk!" << endl << endl;
    cout << "Press CTRL + END to terminate all process instances, if any are running." << endl;
    cout << "Press CTRL + F3 to toggle autokill." << endl;
    cout << "Enter the name of an executable, to check for its running instance: ";
    getline(cin, processName);
    title = processName + " - CheckProcessRunning";
    SetConsoleTitleA(title.c_str());
    TCHAR* processNameParam = new TCHAR[processName.size() + 1];
    processNameParam[processName.size()] = 0;
    copy(processName.begin(), processName.end(), processNameParam);
    bool running;
    thread toggleAutokill(setAutoKill);
    while (true)
    {
        auto now = chrono::system_clock::now();
        time_t t_c = chrono::system_clock::to_time_t(now);
        char* currentTime = ctime(&t_c);
        running = IsProcessRunning(processNameParam);
        if (currentTime[strlen(currentTime) - 1] == '\n') currentTime[strlen(currentTime) - 1] = '\0';
        cout << currentTime << "\t" << running << endl;
        if (!running && wasrunning)
        {
            cout << endl << endl << "Process exited." << endl << endl;
            system("pause");
            exit(0);
        }
    }
}
