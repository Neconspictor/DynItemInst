#include <Configuration.h>
#include <Logger.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <Util.h>

using namespace std;

string Configuration::logFile;
bool Configuration::logInfos;
bool Configuration::logWarnings;
bool Configuration::logErrors;
bool Configuration::logFatals;
bool Configuration::logToConsole;
bool Configuration::logToZSpy;
bool Configuration::logToFile;
bool Configuration::debugisEnabled;
float Configuration::farClipZMultiplicator;

void Configuration::load(const string &filename)
{
	stringstream ss;
	ss << util::getModuleDirectory(util::getModuleHandle()) << "\\" << filename;
	using boost::property_tree::ptree;
	ptree pt;
	try
	{
		read_ini(ss.str(), pt);
		logFile = pt.get<string>("LOGGING.logFile");
		logInfos = pt.get("LOGGING.logInfos", false);
		logWarnings = pt.get("LOGGING.logWarnings", false);
		logErrors = pt.get("LOGGING.logErrors", false);
		logFatals = pt.get("LOGGING.logFatals", false);
		logToZSpy = pt.get("LOGGING.logToZSpy", false);
		logToFile = pt.get("LOGGING.logToFile", false);
		logToConsole = pt.get("LOGGING.logToConsole", false);
		debugisEnabled = pt.get("LOGGING.debugEnabled", false);

	} catch (boost::exception &e)
	{
		//ini couldn't be read; set standard settings
		logFile = "DynItemInst_IkarusLog.txt";
		logInfos = false;
		logWarnings = false;
		logErrors = false;
		logFatals = true;
		debugisEnabled = false;
	}

}

void Configuration::loadLoAIni()
{
	stringstream ss;
	ss << util::getGothicSystemDirectory() << "\\" << "LoA.ini";
	stringstream logStream;
	logStream << "Configuration::loadLoAIni: Try to load LoA.ini in folder: " << ss.str() << endl;
	util::logAlways(&logStream);

	using boost::property_tree::ptree;
	ptree pt;
	try
	{
		read_ini(ss.str(), pt);
		farClipZMultiplicator = pt.get("LOA.farClipZMultiplicator", 1.0f);
		if (farClipZMultiplicator <= 0)
		{
			farClipZMultiplicator = 1.0f;
		}

	}
	catch (boost::exception &e)
	{
		//ini couldn't be read; set standard settings
		farClipZMultiplicator = 1.0f;
	}

	logStream << "Configuration::loadLoAIni: farClipZMultiplicator= " << farClipZMultiplicator << endl;
	util::logAlways(&logStream);

}

void Configuration::save(const string &filename)
{
	stringstream ss;
	ss << util::getModuleDirectory(util::getModuleHandle()) << "\\" << filename;
	using boost::property_tree::ptree;
	ptree pt;
	pt.put("LOGGING.logFile", logFile);
	pt.put("LOGGING.logInfos", logInfos);
	pt.put("LOGGING.logWarnings", logWarnings);
	pt.put("LOGGING.logErrors", logErrors);
	pt.put("LOGGING.logFatals", logFatals);
	pt.put("LOGGING.logToZSpy", logToZSpy);
	pt.put("LOGGING.logToFile", logToFile);
	pt.put("LOGGING.logToConsole", logToConsole);
	pt.put("LOGGING.debugEnabled", debugisEnabled);
	write_ini(ss.str(), pt);
}


string Configuration::getLogFile()
{
	return logFile;
}

float Configuration::getFarClipZMultiplicator()
{
	return farClipZMultiplicator;
}

bool Configuration::debugEnabled()
{
	return debugisEnabled;
}

bool Configuration::getLogToFile()
{
	return logToFile;
}

bool Configuration::getLogTozSpy()
{
	return logToZSpy;
}

bool Configuration::getLogToConsole()
{
	return logToConsole;
}

bool Configuration::getLogInfos()
{
	return logInfos;
};

bool Configuration::getLogWarnings()
{
	return logWarnings;
};

bool Configuration::getLogErrors()
{
	return logErrors;
};

bool Configuration::getLogFatals()
{
	return logFatals;
};