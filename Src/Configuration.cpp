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

#include <Configuration.h>
#include <Logger.h>
#include <Util.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

using namespace std;

Configuration::_Config Configuration::mConfig;


void Configuration::load(const string &filename)
{
	stringstream ss;
	ss << util::getModuleDirectory(util::getModuleHandle()) << "\\" << filename;

	string path = ss.str();

	boost::property_tree::ptree tree;

	try {
		boost::property_tree::ini_parser::read_ini(path, tree);
	}
	catch (const std::exception& e) {
		//we ignore that...
	}
	

	mConfig.logFile = tree.get<std::string>("LOGGING.logFile", "Nec_log.txt");

	mConfig.logInfos = tree.get<bool>("LOGGING.logInfos", false);
	mConfig.logWarnings = tree.get<bool>("LOGGING.logWarnings", false);
	mConfig.logErrors = tree.get<bool>("LOGGING.logErrors", true);
	mConfig.logFatals = tree.get<bool>("LOGGING.logFatals", true);
	mConfig.logToZSpy = tree.get<bool>("LOGGING.logToZSpy", false);
	mConfig.logToFile = tree.get<bool>("LOGGING.logToFile", true);
	mConfig.logToConsole = tree.get<bool>("LOGGING.logToConsole", false);
	mConfig.debugEnabled = tree.get<bool>("LOGGING.debugEnabled", false);
}

void Configuration::save(const string &filename)
{
	stringstream path;
	path << util::getModuleDirectory(util::getModuleHandle()) << "\\" << filename;
	using boost::property_tree::ptree;
	ptree pt;
	pt.put("LOGGING.logFile", mConfig.logFile);
	pt.put("LOGGING.logInfos", mConfig.logInfos);
	pt.put("LOGGING.logWarnings", mConfig.logWarnings);
	pt.put("LOGGING.logErrors", mConfig.logErrors);
	pt.put("LOGGING.logFatals", mConfig.logFatals);
	pt.put("LOGGING.logToZSpy", mConfig.logToZSpy);
	pt.put("LOGGING.logToFile", mConfig.logToFile);
	pt.put("LOGGING.logToConsole", mConfig.logToConsole);
	pt.put("LOGGING.debugEnabled", mConfig.debugEnabled);

	try {
		write_ini(path.str(), pt);
	}
	catch (const std::exception & e) {
		//we ignore that...
	}

	
}


const string& Configuration::getLogFile()
{
	return mConfig.logFile;
}

bool Configuration::debugEnabled()
{
	return mConfig.debugEnabled;
}

bool Configuration::getLogToFile()
{
	return mConfig.logToFile;
}

bool Configuration::getLogTozSpy()
{
	return mConfig.logToZSpy;
}

bool Configuration::getLogToConsole()
{
	return mConfig.logToConsole;
}

bool Configuration::getLogInfos()
{
	return mConfig.logInfos;
};

bool Configuration::getLogWarnings()
{
	return mConfig.logWarnings;
};

bool Configuration::getLogErrors()
{
	return mConfig.logErrors;
};

bool Configuration::getLogFatals()
{
	return mConfig.logFatals;
};