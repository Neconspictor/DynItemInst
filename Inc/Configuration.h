#pragma once
#include <iostream>

class Configuration
{
public:
	static void load(const std::string &filename);
	static void save(const std::string &filename);

	static std::string getLogFile();
	static bool getLogInfos();
	static bool getLogWarnings();
	static bool getLogErrors();
	static bool getLogFatals();
	static float getFarClipZMultiplicator();
	static bool debugEnabled();
	static bool getLogToFile();
	static bool getLogTozSpy();
	static bool getLogToConsole();
	static bool getLoADebug();
private:
	static std::string logFile;
	static bool logInfos;
	static bool logWarnings;
	static bool logErrors;
	static bool logFatals;
	static bool logToConsole;
	static bool logToZSpy;
	static bool logToFile;
	static bool debugisEnabled;
	static float farClipZMultiplicator;
	static bool loaDebug;
};
