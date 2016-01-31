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

class zERROR;

Logger* Logger::instance = nullptr;
std::string Logger::logFileName;

typedef int (__thiscall* ZERROR_REPORT)(void* pThis, int errorType, int, zSTRING const &, signed char, UINT, int, char*, char*);
ZERROR_REPORT zErrorReport;

Logger::Logger()
{
	zErrorReport = (ZERROR_REPORT)ZERROR_REPORT_ADDRESS;
	logFileName = util::getModuleName(util::getModuleHandle()) + std::string("Log");
}

Logger::~Logger()
{
}

Logger* Logger::getLogger()
{
	if (instance == nullptr)
	{
		instance = new Logger();	
		std::string logFilePath = util::getModuleDirectory(util::getModuleHandle()) + std::string("\\") 
			+ logFileName + std::string(".txt");
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
			+ logFileName + std::string(".txt");
	FILE* pFile;
	fopen_s(&pFile, logFilePath.c_str(),"a");
	fputs(message.c_str(), pFile);
	fflush(pFile);
	_commit(_fileno(pFile));
	fclose(pFile);
}

void Logger::writeTozSpy(LogLevel level, std::string message)
{
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

void Logger::log(LogLevel level, std::stringstream* stream, bool toFile, bool tozSpy, bool toConsole)
{
	std::string message = "U: DII-Skript: " + stream->str();
	if (toConsole)
	{
		std::cout << message;		
	}

	if (toFile)
	{
		writeToFile(message);
	}

	if (tozSpy)
	{
		writeTozSpy(level, message);
	}

	// Clear the string stream's content
	stream->clear();
	stream->str("");
};