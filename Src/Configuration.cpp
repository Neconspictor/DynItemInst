#include <Configuration.h>
#include <Logger.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <Util.h>

using namespace std;

string Configuration::logFile;
int Configuration::logLevel = Logger::Fatal;

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
		logLevel = pt.get("LOGGING.logLevel", (int)Logger::Fatal);

	} catch (boost::exception &e)
	{
		//ini couldn't be read; set standard settings
		logFile = "DynItemInst_IkarusLog.txt";
		logLevel = Logger::Fatal;
	}

}

void Configuration::save(const string &filename)
{
	stringstream ss;
	ss << util::getModuleDirectory(util::getModuleHandle()) << "\\" << filename;
	using boost::property_tree::ptree;
	ptree pt;
	pt.put("LOGGING.logFile", logFile);
	pt.put("LOGGING.logLevel", logLevel);
	write_ini(ss.str(), pt);
}

std::string Configuration::getLogFile()
{
	return logFile;
}

int Configuration::getLogLevel()
{
	return logLevel;
}