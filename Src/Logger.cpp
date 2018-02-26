/*////////////////////////////////////////////////////////////////////////////

This file is part of DynItemInst.

Copyright © 2015 David Goeth

All Rights reserved.

THE WORK (AS DEFINED BELOW) IS PROVIDED
UNDER THE TERMS OF THIS CREATIVE COMMONS
PUBLIC LICENSE ("CCPL" OR "LICENSE").
THE WORK IS PROTECTED BY COPYRIGHT AND/OR
OTHER APPLICABLE LAW. ANY USE OF THE WORK
OTHER THAN AS AUTHORIZED UNDER THIS LICENSE
OR COPYRIGHT LAW IS PROHIBITED.

BY EXERCISING ANY RIGHTS TO THE WORK PROVIDED
HERE, YOU ACCEPT AND AGREE TO BE BOUND BY THE
TERMS OF THIS LICENSE. TO THE EXTENT THIS
LICENSE MAY BE CONSIDERED TO BE A CONTRACT,
THE LICENSOR GRANTS YOU THE RIGHTS CONTAINED
HERE IN CONSIDERATION OF YOUR ACCEPTANCE OF
SUCH TERMS AND CONDITIONS.

Full license at http://creativecommons.org/licenses/by-nc/3.0/legalcode

/////////////////////////////////////////////////////////////////////////////*/

#include "Logger.h"
#include "Util.h"
#include <api/g2/ztypes.h>
#include <fstream>
#include <Windows.h>
#include <io.h>
#include <iostream>
#include <sstream>
#include <Configuration.h>
#include <ctime>

class zERROR;

Logger* Logger::instance = NULL;
std::string Logger::logFileName;

typedef int (__thiscall* ZERROR_REPORT)(void* pThis, int errorType, int, zSTRING const &, signed char, UINT, int, char*, char*);
ZERROR_REPORT zErrorReport;

Logger::Logger()
{
	zErrorReport = (ZERROR_REPORT)ZERROR_REPORT_ADDRESS;
}

Logger::~Logger()
{
}

Logger* Logger::getLogger()
{
	if (instance == NULL)
	{
		instance = new Logger();
		Configuration::load("DII_Configuration.ini");
		Configuration::save("DII_Configuration.ini");
		logFileName = Configuration::getLogFile();
		instance->logInfos = Configuration::getLogInfos();
		instance->logWarnings = Configuration::getLogWarnings();
		instance->logErrors = Configuration::getLogErrors();
		instance->logFatals = Configuration::getLogFatals();
		instance->toFile = Configuration::getLogToFile();
		instance->tozSpy = Configuration::getLogTozSpy();
		instance->toConsole = Configuration::getLogToConsole();
		std::string logFilePath = util::getModuleDirectory(util::getModuleHandle()) + std::string("\\") 
			+ logFileName;
		std::ofstream logFile(logFilePath.c_str());
		if (logFile.is_open())
			logFile.close();
	}
	return instance;
}

void Logger::release()
{
	SAFE_DELETE(instance);
}

void Logger::writeToFile(std::string message)
{

	std::string logFilePath = util::getModuleDirectory(util::getModuleHandle()) + std::string("\\") 
			+ logFileName;
	FILE* pFile;
	fopen_s(&pFile, logFilePath.c_str(),"a");
	fputs(message.c_str(), pFile);
	fflush(pFile);
	_commit(_fileno(pFile));
	fclose(pFile);
}

void Logger::writeTozSpy(LogLevel level, std::string message)
{

	message = "U: DII-Skript: " + message;
	zERROR* zerr = (zERROR*)0x008CDCD0;	//zERROR zerr
	zSTRING zMessage = zSTRING(message.c_str());

	switch (level)
	{
	case Info:
			zErrorReport(zerr, level, 0, zMessage, 5, 0, 0x1D1, (char*)0x00893008, NULL);
		break;
	case Warning:
			zErrorReport(zerr, level, 0, zMessage, 0, 0, 0x1DA, (char*)0x00893008, NULL);
		break;
	case Fault:
			zErrorReport(zerr, level, 0, zMessage, 0, 0, 0x1E8, (char*)0x00893008, NULL);

		break;
	case Fatal:
			zErrorReport(zerr, level, 0, zMessage, (signed char)0xFFFFFFFF, 0, 0x1E3, (char*)0x00893008, NULL);
		break;
	}	
}

void Logger::writeToConsole(LogLevel level, std::string message)
{
	std::string timeStamp = getTimeStamp();
	std::cout << "["<< getTimeStamp() << ", " << logLevelToString(level) <<"]    " << message;
}

void Logger::logAlways(std::stringstream* stream)
{
	//make time stamp and add it along with the log level in front of the log message
	std::string timeStamp = getTimeStamp();

	// Clear the util stream content before filling it
	util.clear();
	util.str("");
	util << "[" << timeStamp << "]    "
		<< stream->str();

	std::string message = util.str();

	writeToFile(message);
	writeToConsole(Info, message);
	writeTozSpy(Warning, message);

	// Clear again
	util.clear();
	util.str("");
	stream->clear();
	stream->str("");
}

void Logger::log(LogLevel level, std::stringstream* stream)
{
	//early exit
	if (!isLogLevelActive(level)) { return; }

	std::string message = stream->str();
	if (toConsole)
	{
		writeToConsole(level, message);	
	}

	if (toFile)
	{
		writeToFile(createOutputWithLogLevel(level, message));
	}

	if (tozSpy)
	{
		writeTozSpy(level, message);
	}

	// Clear the string stream's content
	stream->clear();
	stream->str("");
}

bool Logger::isLogLevelActive(LogLevel level)
{
	switch (level) {
	case Info:
		if (logInfos)
		{
			return true;
		}
		break;
	case Warning:
		if (logWarnings)
		{
			return true;
		}
		break;
	case Fault:
		if (logErrors)
		{
			return true;
		}
		break;
	case Fatal:
		if (logFatals)
		{
			return true;
		}
		break;
	default:
		std::stringstream ss; ss << "Logger::isLogLevelActive: unknown log level: " << level << std::endl;
		logAlways(&ss);
		break;
	}

	//log level wasn't found
	return false;
}

std::string Logger::createOutputWithLogLevel(LogLevel level, std::string message)
{
	//make time stamp and add it along with the log level in front of the log message
	std::string timeStamp = getTimeStamp();
	std::string logLevelDesc = logLevelToString(level);

	// Clear the util stream content before filling it
	util.clear();
	util.str("");
	util << "[" << timeStamp << ", " << logLevelDesc << "]    "
		<< message;

	return util.str();
}

std::string Logger::logLevelToString(LogLevel level)
{
	switch(level) {
	case Info:
		return "INFO";
	case Warning:
		return "WARNING";
	case Fault:
		return "FAULT";
	case Fatal:
		return "FATAL";
	default:
		std::stringstream ss; ss << "Logger::logLevelToString: unknown log level: " << level << std::endl;
		logAlways(&ss);
	}
	return "";
};

std::string Logger::getTimeStamp()
{
	tm currTime;
	time_t timeStamp = time(NULL);
	//currTime = localtime(&timeStamp);
	localtime_s(&currTime, &timeStamp);

	// Clear the util stream content before filling it
	util.clear();
	util.str("");
	util << currTime.tm_hour << ":" << currTime.tm_min << ":" << currTime.tm_sec;

	return util.str();
};