#include <utility/CCXConsoleColour.h>

#include <unordered_map>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

const std::unordered_map<CCX::ConsoleColour, int> WinForegroundColour
{
    { CCX::ConsoleColour::Black, 0 },
    { CCX::ConsoleColour::Red, FOREGROUND_RED },
    { CCX::ConsoleColour::Green, FOREGROUND_GREEN },
    { CCX::ConsoleColour::Yellow, FOREGROUND_RED | FOREGROUND_GREEN },
    { CCX::ConsoleColour::Blue, FOREGROUND_BLUE },
    { CCX::ConsoleColour::Magenta, FOREGROUND_RED | FOREGROUND_BLUE },
    { CCX::ConsoleColour::Cyan, FOREGROUND_BLUE | FOREGROUND_GREEN },
    { CCX::ConsoleColour::White, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN },
    { CCX::ConsoleColour::Default, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN }
};

const std::unordered_map<CCX::ConsoleColour, int> WinBackgroundColour
{
    { CCX::ConsoleColour::Black, 0 },
    { CCX::ConsoleColour::Red, BACKGROUND_RED },
    { CCX::ConsoleColour::Green, BACKGROUND_GREEN },
    { CCX::ConsoleColour::Yellow, BACKGROUND_RED | BACKGROUND_GREEN },
    { CCX::ConsoleColour::Blue, BACKGROUND_BLUE },
    { CCX::ConsoleColour::Magenta, BACKGROUND_RED | BACKGROUND_BLUE },
    { CCX::ConsoleColour::Cyan, BACKGROUND_BLUE | BACKGROUND_GREEN },
    { CCX::ConsoleColour::White, BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN },
    { CCX::ConsoleColour::Default, BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN }
};

bool IsInitialised = false;
HANDLE Console;

void Initialise()
{
    if (IsInitialised) return;
    Console = GetStdHandle(STD_OUTPUT_HANDLE);
    IsInitialied = true;
}

void CCX::SetConsoleColour(CCX::ConsoleColour foreground, CCX::ConsoleColour background) {
    Initialise();
    int foregroundCode = WinForegroundColour.at(foreground);
    int backgroundCode = WinBackgroundColour.at(background);
    SetConsoleTextAttribute(console, foreground | background);
}

#elif defined(__linux__)
#include <iostream>

const std::unordered_map<CCX::ConsoleColour, int> LinuxColour
{
    { CCX::ConsoleColour::Black, 30 },
    { CCX::ConsoleColour::Red, 31 },
    { CCX::ConsoleColour::Green, 32 },
    { CCX::ConsoleColour::Yellow, 33 },
    { CCX::ConsoleColour::Blue, 34 },
    { CCX::ConsoleColour::Magenta, 35 },
    { CCX::ConsoleColour::Cyan, 36 },
    { CCX::ConsoleColour::White, 37 },
    { CCX::ConsoleColour::Default, 39 }
};

void CCX::SetConsoleColour(CCX::ConsoleColour foreground, CCX::ConsoleColour background) {
    int foregroundCode = LinuxColour.at(foreground);
    int backgroundCode = LinuxColour.at(background) + 10;
    std::cout << u"\x1b[" << foregroundCode << ";" << backgroundCode << "m";
}

#endif