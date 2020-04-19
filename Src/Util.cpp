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

#include "Util.h"
#include <Windows.h>
#include <Shellapi.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> // stat
#include <errno.h>    // errno, ENOENT, EEXIST
#include <Logger.h>
#if defined(_WIN32)
#include <direct.h>   // _mkdir
#endif
#include <sstream>
#include <Configuration.h>
#include "api/g2/ztypes.h"
#include <zCPar_SymbolTable.h>
#include <ObjectManager.h>
#include <api/g2/zcparser.h>
#include <Util_Constants.h>
#include <random>


HMODULE util::hModule = 0;
std::stringstream util::mLogStream;

void util::safeDelete(void** pointer)
{
	if (*pointer != NULL)
	{
		delete *pointer;
		*pointer = NULL;
	}
}

int util::generateRandom(int min, int max)
{
	std::random_device random_device;
	std::mt19937 random_engine(random_device());
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(random_engine);
}

std::string util::getCurrentWorkingDir()
{
	char buffer[MAX_PATH];
    GetModuleFileName( NULL, buffer, MAX_PATH );
	std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
    return std::string( buffer ).substr( 0, pos);
}

std::string util::getModuleName(HMODULE handle)
{
	char buffer[MAX_PATH];
	GetModuleFileName(handle, buffer, sizeof(buffer)/sizeof(buffer[0]));
	std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
	std::string result = std::string( buffer ).substr(pos +1);
	pos = result.find_last_of( "." );
	return result.substr(0, pos);
}

std::string util::getModuleDirectory(HMODULE handle)
{
	char buffer[MAX_PATH];
	GetModuleFileName(handle, buffer, sizeof(buffer)/sizeof(buffer[0]));
	std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
    return std::string( buffer ).substr( 0, pos);
}

void util::setModuleHandle(HMODULE handle)
{
	hModule = handle;
}

HMODULE util::getModuleHandle()
{
	return hModule;
}

std::string util::getGothicSystemDirectory()
{
	return getCurrentWorkingDir();
}

void util::assertDIIRequirements(bool expression, std::string errorMessage)
{
	if (!expression)
	{
		mLogStream << "DII-Assertion failed: " << errorMessage << std::endl;
		Logger::getLogger()->log(Logger::Fatal, &mLogStream);
	}
}

void util::debug(std::stringstream* ss, Logger::LogLevel level)
{
	if (Configuration::debugEnabled())
	{
		Logger::getLogger()->log(level, ss);
	} else
	{
		//flush strin stream
		ss->clear();
		ss->str("");
	}
}


void util::logAlways(std::stringstream* ss)
{
	Logger::getLogger()->logAlways(ss);
}

void util::callDaedalusFunction_Int2(std::string functionName, int first, int second, bool isExternal)
{
	zCParser* parser = zCParser::GetParser();
	int index = parser->GetIndex(zSTRING(functionName.c_str()).Upper());

	bool hasReturn = false;
	void* dataStack = (BYTE*)parser + 0x58;

	g2ext_extended::zCPar_SymbolTable* currSymbolTable = ObjectManager::zCParserGetSymbolTable(parser);

	zCPar_Symbol* symbol = currSymbolTable->GetSymbol(index);
	int stackPosition = 0;
	*(int*)((BYTE*)parser + 0x219C) = index;
	constants::zCPar_SymbolGetStackPos(symbol, stackPosition, 0);

	constants::zCPar_StackClear(dataStack);

	// push arguments on data stack
	constants::zCPar_StackPush(dataStack, first);
	constants::zCPar_StackPush(dataStack, constants::ZCPAR_TOK_PUSHINT);

	constants::zCPar_StackPush(dataStack, second);
	constants::zCPar_StackPush(dataStack, constants::ZCPAR_TOK_PUSHINT);

	if (isExternal)
	{
		typedef int(__cdecl* External)();
		External external = (External)stackPosition;
		external();
	}
	else
	{
		constants::zCParserDoStack(parser, stackPosition);
	}

	if (!hasReturn)
	{
		return;
	}

	// pop return value
	int result = constants::zCPar_DataStackPop(dataStack);
	for (int i = 0; i < 1; ++i)
	{
		result = constants::zCPar_DataStackPop(dataStack);
	}

	*(int*)((BYTE*)parser + 0x219C) = -1;
}

std::string util::trimFromRight(const std::string& str)
{
	// trim trailing spaces
	size_t endpos = str.find_last_not_of(" ");

	if (endpos < 0 || endpos > str.length()) return str;

	if (str[endpos] == ' ')
	{
		return str.substr(0, endpos);
	}
	return str.substr(0, endpos + 1);
}

void util::readString(std::stringstream* is, std::string& data)
{
	while (is->peek() == ' ')
	{
		is->get();
	}

	int size = 0;
	getInt(*is, size);
	if (size)
	{
		data.resize(size);
		is->get(&data[0], size +1);
	}
	else
	{
		data = "";
	}
	data = trimFromRight(data);
}

void util::readAndTrim(std::stringstream* is, std::string& data)
{
	readString(is, data);
	data = trimFromRight(data);
}

void util::writeString(std::ostream& os, const std::string& data)
{
	// c-strings avoid Nullbytes to be written
	char const* content = data.c_str();
	os << std::strlen(content) << ' ';
	os << content;
}

void util::getInt(std::stringstream& ss, int& param)
{
	while (ss.peek() == ' ')
	{
		ss.get();
	}

	std::string token;
	getline(ss, token, ' ');
	param = atoi(token.c_str());
};

void util::getBool(std::stringstream& ss, bool& param)
{
	while (ss.peek() == ' ')
	{
		ss.get();
	}
	std::string token;
	getline(ss, token, ' ');
	param = atoi(token.c_str()) != 0;
};

void util::logInfo(std::stringstream* ss)
{
	Logger::getLogger()->log(Logger::Info, ss);
}

void util::logWarning(std::stringstream* ss)
{
	Logger::getLogger()->log(Logger::Warning, ss);
}

void util::logFault(std::stringstream* ss)
{
	Logger::getLogger()->log(Logger::Fault, ss);
}

void util::logFatal(std::stringstream* ss)
{
	Logger::getLogger()->log(Logger::Fatal, ss);
}



bool util::areEqualIgnoringCases(const std::string& a, const std::string& b)
{
	std::string aUpper = a;
	std::string bUpper = b;
	std::transform(aUpper.begin(), aUpper.end(), aUpper.begin(), std::toupper);
	std::transform(bUpper.begin(), bUpper.end(), bUpper.begin(), std::toupper);
	return aUpper == bUpper;
}

bool util::existsDir(const std::string& path)
{
	DWORD attributes = GetFileAttributes(path.c_str());
	if (attributes == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}

	if (!(attributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		mLogStream << "util::existsDir: " << path << " exists but isn't a directory!" << std::endl;
		logFault(&mLogStream);
	}

	return true;


#if defined(_WIN32)
    struct _stat info;
    if (_stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & _S_IFDIR) != 0;
#else 
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
#endif
}

bool util::makePath(const std::string& path)
{
#if defined(_WIN32)
    int ret = _mkdir(path.c_str());
#else
    mode_t mode = 0755;
    int ret = mkdir(path.c_str(), mode);
#endif
    if (ret == 0)
        return true;

    switch (errno)
    {
    case ENOENT:
        // parent didn't exist, try to create it
        {
            int pos = path.find_last_of('/');
            if (pos == std::string::npos)
#if defined(_WIN32)
                pos = path.find_last_of('\\');
            if (pos == std::string::npos)
#endif
                return false;
            if (!makePath( path.substr(0, pos) ))
                return false;
        }
        // now, try to create again
#if defined(_WIN32)
        return 0 == _mkdir(path.c_str());
#else 
        return 0 == mkdir(path.c_str(), mode);
#endif

    case EEXIST:
        // done!
        return existsDir(path);

    default:
        return false;
    }
}


void util::split(std::vector<std::string> &tokens, const std::string &text, char sep) {
  int start = 0, end = 0;
  while ((end = text.find(sep, start)) != std::string::npos) {
    tokens.push_back(text.substr(start, end - start));
    start = end + 1;
  }
  tokens.push_back(text.substr(start));
};



void util::copyContentTo(std::string source, std::string dest, std::string pattern) {

	std::string sourceFinal = source + pattern;
	sourceFinal.append(1,'\0');
	std::string destFinal = dest;// + std::string(1,'\0');
	destFinal.append(1,'\0');

	SHFILEOPSTRUCT s = { 0 };
	s.wFunc = FO_COPY;
	s.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_SILENT | FOF_NOERRORUI;
	s.pTo = destFinal.c_str();//(LPWSTR)(destFinal.c_str());
	s.pFrom = sourceFinal.c_str();//(LPWSTR)(sourceFinal.c_str());
	int error = SHFileOperation(&s);

	if (error)
	{
		mLogStream << "util::copyContentTo: Couldn't copy files from " << source << " to "
			<< dest << std::endl;
		logFault(&mLogStream);
	}

	if (s.fAnyOperationsAborted)
	{
		mLogStream << "util::copyContentTo: Any operation was aborted while copying " << source << " to "
			<< dest << std::endl;
		logFault(&mLogStream);
	}
}

void util::deleteAllFiles(std::string folder, std::string pattern)
{
	std::string sourceFinal = folder + pattern;
	sourceFinal.append(1,'\0');

	SHFILEOPSTRUCT s = { 0 };
	s.wFunc = FO_DELETE;
	s.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_SILENT | FOF_NOERRORUI;
	s.pFrom = sourceFinal.c_str();//(LPWSTR)(sourceFinal.c_str());
	int error = SHFileOperation(&s);

	if (error)
	{
		mLogStream << "util::deleteAllFiles: Couldn't delete files from " << folder << " with pattern "
			<< pattern << std::endl;
		logFault(&mLogStream);
	}

	if (s.fAnyOperationsAborted)
	{
		mLogStream << "util::deleteAllFiles: Any operation was aborted while deleting files from " << folder << " with pattern "
			<< pattern << std::endl;
		logFault(&mLogStream);
	}
};

void util::copyFileTo(std::string from, std::string to){

	//do not copy if source and destination are equal
	if (from.compare(to) == 0)
	{
		mLogStream << "util::copyFileTo: No copying is done, as source and destination are equal:" << std::endl;
		mLogStream << "\tsource: " << from << std::endl;
		mLogStream << "\tdestination: " << to << std::endl;
		logWarning(&mLogStream);
		return;
	}

	BOOL success = CopyFile( from.c_str(), to.c_str(), FALSE );

	if (!success)
	{
		DWORD errorCode = GetLastError();

		LPTSTR errorText = NULL;

		FormatMessage(
			// use system message tables to retrieve error text
			FORMAT_MESSAGE_FROM_SYSTEM
			// allocate buffer on local heap for error text
			| FORMAT_MESSAGE_ALLOCATE_BUFFER
			// Important! will fail otherwise, since we're not 
			// (and CANNOT) pass insertion parameters
			| FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
			errorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&errorText,  // output 
			0, // minimum size for output buffer
			NULL);   // arguments - see note 

		if (NULL != errorText)
		{
			mLogStream << "util::copyFileTo: While copying from " << from << " to " << to << 
				" an error occured: " << errorText << std::endl;
			logFault(&mLogStream);

			// release memory allocated by FormatMessage()
			LocalFree(errorText);
			errorText = NULL;
		}
	}
};