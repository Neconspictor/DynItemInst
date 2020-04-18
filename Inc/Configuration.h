#pragma once
#include <iostream>

class Configuration
{
public:
	static void load(const std::string &filename);
	static void save(const std::string &filename);

	static const std::string& getLogFile();
	static bool getLogInfos();
	static bool getLogWarnings();
	static bool getLogErrors();
	static bool getLogFatals();
	static bool debugEnabled();
	static bool getLogToFile();
	static bool getLogTozSpy();
	static bool getLogToConsole();
private:

	struct _Config
	{
		std::string logFile = "Nec_log.txt";
		bool logInfos = false;
		bool logWarnings = false;
		bool logErrors = true;
		bool logFatals = true;
		bool logToConsole = false;
		bool logToZSpy = false;
		bool logToFile = true;
		bool debugEnabled = false;
	};

	static _Config mConfig;
};
