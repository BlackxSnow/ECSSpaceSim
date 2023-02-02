#include <utility/ConsoleLogging.h>
#include <utility/CCXConsoleColour.h>
#include <iostream>

#include <ctime>

CCX::LogFlags CCX::LogSettings = CCX::LogFlags::Time | CCX::LogFlags::SourceInfo;

void LayoutPrefix(int line, const std::string& func, const std::string& file, std::string& output)
{
	if ((int)CCX::LogSettings != 0)
	{
		if (CCX::LogSettings & CCX::LogFlags::Time)
		{
			std::time_t t = std::time(nullptr);
            char* buffer = new char[8];
            std::strftime(buffer, 8, "%T", std::gmtime(&t));

//            std::asctime(std::localtime(&t));

//			struct tm timeInfo;
//			localtime_s(&timeInfo, &t);

//			std::string buffer;
//			buffer.resize(32);
//			int len = strftime(&buffer[0], buffer.size(), "%X", &timeInfo);
//			while (len == 0) {
//				buffer.resize(buffer.size() * 2);
//				len = strftime(&buffer[0], buffer.size(), "%X", &timeInfo);
//			}
//
//			size_t lastNonSpace = buffer.find_last_not_of(" \0", buffer.length(), 2);
//			buffer.erase(buffer.begin() + lastNonSpace + 1, buffer.end());

            output += "[";
			output.append(buffer);
            output += "]";
		}
		if (CCX::LogSettings & CCX::LogFlags::SourceInfo)
		{
			output += "(Line " + std::to_string(line) + " @ " + func + " @ " + file + ")";
		}
		output += " ";
	}
}

void CCX::Assert(int line, std::string sourceFunc, std::string sourceFile, bool assertion, std::string message = "")
{
	if (!assertion)
	{
		Error(line, sourceFunc, sourceFile, message, true);
	}
}

void CCX::Error(int line, std::string sourceFunc, std::string sourceFile, std::string message, bool throwException)
{
	CCX::SetConsoleColour(CCX::ConsoleColour::Red, CCX::ConsoleColour::Default);
	std::string output = "[ERROR] ";
	LayoutPrefix(line, sourceFunc, sourceFile, output);
	std::cout << output << message << "\n";
	if (throwException)
	{
		std::cerr << message;
		throw std::runtime_error(message);
	}
}

void CCX::Warning(int line, std::string sourceFunc, std::string sourceFile, std::string message)
{
    CCX::SetConsoleColour(CCX::ConsoleColour::Yellow, CCX::ConsoleColour::Default);
	std::string output = "[WARN] ";
	LayoutPrefix(line, sourceFunc, sourceFile, output);
	std::cout << output << message << "\n";
}

void CCX::Info(int line, std::string sourceFunc, std::string sourceFile, std::string message)
{
    CCX::SetConsoleColour(CCX::ConsoleColour::Default, CCX::ConsoleColour::Default);
	std::string output = "[INFO] ";
	LayoutPrefix(line, sourceFunc, sourceFile, output);
	std::cout << output << message << "\n";
}
