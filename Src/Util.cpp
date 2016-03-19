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

#include "Util.h"
#include <Windows.h>
#include <Shellapi.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h> // stat
#include <errno.h>    // errno, ENOENT, EEXIST
#include <Logger.h>
#if defined(_WIN32)
#include <direct.h>   // _mkdir
#endif
#include <sstream>
#include <Configuration.h>


HMODULE util::hModule = 0;
std::stringstream util::logStream;

void util::safeDelete(void** pointer)
{
	if (*pointer != NULL)
	{
		delete *pointer;
		*pointer = NULL;
	}
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
		logStream << "DII-Assertion failed: " << errorMessage << std::endl;
		Logger::getLogger()->log(Logger::Fatal, &logStream);
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



bool util::existsDir(const std::string& path)
{
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

	SHFILEOPSTRUCTA s = { 0 };
	s.wFunc = FO_COPY;
	s.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_SILENT;;
	s.pTo = destFinal.c_str();//(LPWSTR)(destFinal.c_str());
	s.pFrom = sourceFinal.c_str();//(LPWSTR)(sourceFinal.c_str());
	SHFileOperationA(&s);
}

void util::deleteAllFiles(std::string folder, std::string pattern)
{
	std::string sourceFinal = folder + pattern;
	sourceFinal.append(1,'\0');

	SHFILEOPSTRUCTA s = { 0 };
	s.wFunc = FO_DELETE;
	s.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_SILENT;;
	s.pFrom = sourceFinal.c_str();//(LPWSTR)(sourceFinal.c_str());
	SHFileOperationA(&s);
};

void util::copyFileTo(std::string from, std::string to){
	CopyFileA( from.c_str(), to.c_str(), FALSE );	
};