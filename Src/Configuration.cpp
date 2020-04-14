#include <Configuration.h>
#include <Logger.h>
#include <Util.h>
#include <ini.h>

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


typedef struct
{
	const char* logFile;
	bool logInfos;
	bool logWarnings;
	bool logErrors;
	bool logFatals;
	bool logToConsole;
	bool logToZSpy;
	bool logToFile;
	bool debugisEnabled;
	float farClipZMultiplicator;
	bool loaDebug;
} configuration;

static int handler(void* user, const char* section, const char* name,
	const char* value)
{
/*	configuration* pconfig = (configuration*)user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
	if (MATCH("LOGGING", "logFile")) {
		pconfig->logFile = strdup(value);
	}
	else if (MATCH("LOGGING", "logInfos")) {
		pconfig->logInfos = atoi(value);
	}
	else if (MATCH("LOGGING", "logWarnings")) {
		pconfig->logWarnings = atoi(value);
	}
	else if (MATCH("LOGGING", "logErrors")) {
		pconfig->logErrors = atoi(value);
	}
	else if (MATCH("LOGGING", "logFatals")) {
		pconfig->logFatals = atoi(value);
	}
	else if (MATCH("LOGGING", "logToZSpy")) {
		pconfig->logToZSpy = atoi(value);
	}
	else if (MATCH("LOGGING", "logToFile")) {
		pconfig->logToFile = atoi(value);
	}
	else if (MATCH("LOGGING", "logToConsole")) {
		pconfig->logToConsole = atoi(value);
	}
	else if (MATCH("LOGGING", "debugEnabled")) {
		pconfig->debugisEnabled = atoi(value);
	}
	else if (MATCH("DEBUG", "loaDebug")) {
		pconfig->loaDebug = atoi(value);
	}
	else {
		return 0;  // unknown section/name, error
	}*/
	return 1;
}


void Configuration::load(const string &filename)
{
	stringstream ss;
	ss << util::getModuleDirectory(util::getModuleHandle()) << "\\" << filename;

	string path = ss.str();

	//ini couldn't be read; set standard settings
	logFile = "NecPack_log.txt";
	logInfos = true;
	logWarnings = true;
	logErrors = true;
	logFatals = true;
	debugisEnabled = true;

	logToZSpy = true;
	logToFile = true;
	logToConsole = true;

	configuration config;
	if (ini_parse("test.ini", handler, &config) < 0) {
		printf("Can't load '%s'\n", path.c_str());
	}
}

void Configuration::save(const string &filename)
{
	/*stringstream ss;
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
	pt.put("DEBUG.loaDebug", loaDebug);
	write_ini(ss.str(), pt);*/
}


string Configuration::getLogFile()
{
	return logFile;
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