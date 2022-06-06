#include <utility/ConsoleLogging.h>
#include <iostream>
#include <Windows.h>

#include <ctime>
#include <time.h>

HANDLE console;
bool isInitialised = false;

clog::LogFlags clog::LogSettings = clog::LogFlags::Time | clog::LogFlags::SourceInfo;

void Init()
{
	if (!isInitialised)
	{
		console = GetStdHandle(STD_OUTPUT_HANDLE);
		isInitialised = true;
	}
}

void LayoutPrefix(int line, const std::string& func, const std::string& file, std::string& output)
{
	if ((int)clog::LogSettings != 0)
	{
		if (clog::LogSettings & clog::LogFlags::Time)
		{
			std::time_t t = std::time(nullptr);

			struct tm timeInfo;
			localtime_s(&timeInfo, &t);

			std::string buffer;
			buffer.resize(32);
			int len = strftime(&buffer[0], buffer.size(), "%X", &timeInfo);
			while (len == 0) {
				buffer.resize(buffer.size() * 2);
				len = strftime(&buffer[0], buffer.size(), "%X", &timeInfo);
			}
			
			size_t lastNonSpace = buffer.find_last_not_of(" \0", buffer.length(), 2);
			buffer.erase(buffer.begin() + lastNonSpace + 1, buffer.end());

			output += "[" + buffer +"]";
		}
		if (clog::LogSettings & clog::LogFlags::SourceInfo)
		{
			output += "(Line " + std::to_string(line) + " @ " + func + " @ " + file + ")";
		}
		output += " ";
	}
}

void clog::Assert(int line, std::string sourceFunc, std::string sourceFile, bool assertion, std::string message = "")
{
	if (!assertion)
	{
		Error(line, sourceFunc, sourceFile, message, true);
	}
}

void clog::Error(int line, std::string sourceFunc, std::string sourceFile, std::string message, bool throwException)
{
	Init();
	SetConsoleTextAttribute(console, FOREGROUND_RED);
	std::string output = "[ERROR] ";
	LayoutPrefix(line, sourceFunc, sourceFile, output);
	std::cout << output << message << "\n";
	if (throwException)
	{
		std::cerr << message;
		throw std::runtime_error(message);
	}
}

void clog::Warning(int line, std::string sourceFunc, std::string sourceFile, std::string message)
{
	Init();
	SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN);
	std::string output = "[WARN] ";
	LayoutPrefix(line, sourceFunc, sourceFile, output);
	std::cout << output << message << "\n";
}

void clog::Info(int line, std::string sourceFunc, std::string sourceFile, std::string message)
{
	Init();
	SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::string output = "[INFO] ";
	LayoutPrefix(line, sourceFunc, sourceFile, output);
	std::cout << output << message << "\n";
}
