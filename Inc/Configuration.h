#pragma once
#include <iostream>

class Configuration
{
public:
	static void load(const std::string &filename);
	static void save(const std::string &filename);

	static std::string getLogFile();
	static int getLogLevel();
private:
	static std::string logFile;
	static int logLevel;
};
