#pragma once
/*////////////////////////////////////////////////////////////////////////////

This file is part of neclib.

Copyright © 2015-2020 David Goeth

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

/////////////////////////////////////////////////////////////////////////////**/

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
