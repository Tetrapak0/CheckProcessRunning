#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>
#include <chrono>
#include <string>
#include <cctype>
#include <algorithm>

#pragma warning(disable : 4996)

using namespace std;

bool wasrunning;
string str;
bool autoKill;
bool killed = false;

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
            string endCommand = "taskkill /f /im \"" + str + "\"";
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
    cout << "Enter the name of an executable, to check for its running instance: ";
    getline(cin, str);
    string choice;
    do
    {
        cout << "Do you want to automatically terminate all process instances in case any are running? [Y/n]";
        getline(cin, choice);
        transform(choice.begin(), choice.end(), choice.begin(), [](unsigned char c) { return tolower(c); });
        if (choice == "" || choice == "y")
        {
            autoKill = true;
            break;
        }
        else if (choice == "n")
        {
            autoKill = false;
            break;
        }
    } while (true);
    string newtitle = str + " - CheckProcessRunning";
    SetConsoleTitleA(newtitle.c_str());
    TCHAR* param = new TCHAR[str.size() + 1];
    param[str.size()] = 0;
    copy(str.begin(), str.end(), param);
    bool running;
    while (true)
    {
        auto now = chrono::system_clock::now();
        time_t t_c = chrono::system_clock::to_time_t(now);
        char* currentTime = ctime(&t_c);
        running = IsProcessRunning(param);
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
