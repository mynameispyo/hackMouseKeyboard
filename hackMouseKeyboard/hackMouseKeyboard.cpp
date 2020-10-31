#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <sstream>
#include <thread>
#include <chrono>
using namespace std;

void click(int x, int y) {
    cout << "clicked : " << x << ", " << y << endl;

    INPUT input;
    ZeroMemory(&input, sizeof(input));      // reset input variable

    // 1. move the pointer
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    float dx = (float)x * 65353.f / 1000;
    float dy = (float)y * 65353.f / 1000;
    input.mi.dx = LONG(dx);
    input.mi.dy = LONG(dy);
    SendInput(1, &input, sizeof(input));        // send move moving information to operating system (windows 10)


    // 2. left click
    ZeroMemory(&input, sizeof(input));      // reset input variable
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(input));        // send left click information to operating system

    // 3. release
    ZeroMemory(&input, sizeof(input));      // reset input variable
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(input));       // send mmouse release information to operating system
}

void type_key(char c) {
    cout << c << " typed" << endl;
    INPUT input;
    ZeroMemory(&input, sizeof(input));
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = VkKeyScanA(c);
    SendInput(1, &input, sizeof(input));

    ZeroMemory(&input, sizeof(input));
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(input));

}

// bring the target window (given process id) to the top
BOOL SetFocusWindowByPID(DWORD dwProcessID) {
    BOOL bResult = FALSE;
    HWND h = GetTopWindow(0);
    while (h) {
        DWORD pid = 0;
        DWORD dwThreadId = GetWindowThreadProcessId(h, &pid);

        // check whether current window handler is what I am looking for
        if (dwThreadId != 0) {
            if (pid == dwProcessID) {
                bResult = SetForegroundWindow(h);  // move the window to the top

                click(400, 400);
                return bResult;
            }
        }
        // go to next window
        h = GetNextWindow(h, GW_HWNDNEXT);
    }

    return FALSE;
}

int main()
{
    cout << "Start program...." << endl;

    wstring targetProcessName = L"notepad.exe";
    cout << "What I am looking for is..";
    wcout << targetProcessName << endl;

    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);   // get all processes (snapshot)
    PROCESSENTRY32W current_process;  // current process
    current_process.dwSize = sizeof current_process;

    if (!Process32FirstW(snap, &current_process)) {  // start with the first process in snapshot
        return 0;
    }

    do {
        //DWORD pid = current_process.th32ProcessID;
        //cout << "current process id : " << pid << endl;

        // check whether the current process matches with the target process
        if (wstring(current_process.szExeFile) == targetProcessName) {
            DWORD process_id = current_process.th32ProcessID;
            cout << "You found it! : " << process_id << endl;

            if (SetFocusWindowByPID(process_id)) {
                cout << "Yeah! I move notepad to the top!" << endl;
                break;
            }
        }
    } while (Process32NextW(snap, &current_process));


    // press 'a' key on notepad (ascii code of 'a' is 97)
    //type_key(char(97));
    //type_key(char(98));

    // Mission: type random characters (50 differnt random keys) to notepad
    for (int i = 0; i < 200; i++) {
        this_thread::sleep_for(chrono::milliseconds(100));
        type_key((char)(97 + (rand() % 26)));   // 97-122 (a-z)

        if (i % 10 == 0) {
            type_key((char)13);  // RETURN key
        }
    }


    system("pause");

    return 0;
}