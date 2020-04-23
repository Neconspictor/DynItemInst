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

#pragma once
#define SAFE_DELETE(pointer) util::safeDelete(reinterpret_cast<void**>(&pointer));

#include <vector>
#include <Windows.h>
#include "Logger.h"
#include <list>
#include "api/g2/ztypes.h"
#include <api/g2/macros.h>
#include <api/g2/zcparser.h>
#include <api/g2/zcpar_symbol.h>

#define UTIL_GET_SYMBOL_WITH_CHECKS(x) util::getSymbolWithChecks(x, __FUNCTION__)

#define LEGO_HOOKENGINE_PREAMBLE __asm      \
/* Port output */         \
{                         \
/*96 Bytes for hook engine stuff*/\
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
						  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
						  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
						  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
						  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
						  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
						  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
						  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
						  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
						  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
						  \
/*5 Bytes for jump instruction*/\
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
};



#define LEGO_HOOKENGINE_PREAMBLE_2_5 __asm      \
/* Port output */         \
{                         \
/*25 Bytes for hook engine stuff*/\
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
						  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
						  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
   __asm nop			  \
};


/**
* Contains functions that are useful for several classes.
*/
class util {
public:


    static bool areEqualIgnoringCases(const std::string& a, const std::string& b);


	/**
	 * Checks if the specified path is an directory and exists.
	 * \param path The directory path to check.
	 * \return Exists the specified directory path?
	 */
	static bool existsDir(const std::string& path);

	/**
	 * Creates the provied folder structure.
	 * \param path The directory path to create
	 * \return Was the path creation successful?
	 */
	static bool makePath(const std::string& path);

	/**
	 * Splits the given text 'text' on the base of the separation character 'sep'
	 * \param tokens A vector which will be used to store all substrings that were created 
	 * by seperating the given text.
	 * \param text The text that should be splitted.
	 * \param sep Always this character was found in the text the latter will be splitted at the
	 * current position.
	 */
	static void split(std::vector<std::string> &tokens, const std::string &text, char sep);

	/**
	 * Copies all files from a specified source path which additionally begin with the pattern
	 * 'pattern' and moves the result to the destination folder 'dest'.
	 * \param source The directory to copy from
	 * \param des The directory to copy to
	 * \param pattern Only files, which names begin with this pattern, will be treated.
	 */
	static void copyContentTo(std::string source, std::string dest, std::string pattern);

	/**
	 * Deletes all files from the folder 'folder' which begin with the pattern 'pattern'.
	 * \param folder The directory to delete the files from.
	 * \param pattern Only files, which names begin with this pattern, will be treated.
	 */
	static void deleteAllFiles(std::string folder, std::string pattern);

	/**
	 * Copies a file and moves its copy to a specified destination.
	 * \param from The file to be copied.
	 * \param to The file path the file should be copied to.
	 */
	static void copyFileTo(std::string from, std::string to);

	/**
	 * Takes the address of an pointer and deletes the pointer and sets its content to NULL.
	 * \param address The address of the pointer
	 */
	static void safeDelete(void** address);

    static int generateRandom(int min, int max);

	/**
	 * Provides the pth to the current working directory of this application.
	 * \return The current working directory
	 */
	static std::string getCurrentWorkingDir();

	/**
	 * Provides the name of a module referenced by a given handle.
	 * \param handle The handle to the module for which the name is desired
	 * \return The name of the module referenced by the module handle.
	 */
	static std::string getModuleName(HMODULE handle);

	/**
	 * Provides the directory path of a module referenced by a given handle.
	 * \param handle The handle to the module for which the directory path is desired
	 * \return The directory path of the module referenced by the module handle.
	 */
	static std::string getModuleDirectory(HMODULE handle);

	/**
	 * Sets the module handle for this namespace.
	 * \param handle The handle which should be assigned to the namespace module handle.
	 */
	static void setModuleHandle(HMODULE handle);

	/**
	 * Provides the module handle of this namespace.
	 * Attention: In order to work properly, the method util::setModuleHandle has to have been called before.
	 * \return The module handle hold by this namespace.
	 */
	static HMODULE getModuleHandle();



    /**
     * Provides the name of a symbol identified by its parser symbol index.
     * If the index isn't valid, the empty string is returned.
     */
    static const zSTRING& getSymbolName(int index);

    static inline int getIndexDefaultZero(const zSTRING& name) {
        zCParser* parser; parser = zCParser::GetParser();
        int index = parser->GetIndex(name);
        return index == -1 ? 0 : index;
    }

    static inline int getIndexWithChecks(const zSTRING& name, const std::string& callerDescription) {
        zCParser* parser; parser = zCParser::GetParser();
        int index = parser->GetIndex(name);

        if (index == -1) {
            std::stringstream mLogStream;
            mLogStream << callerDescription << ": '" << name.ToChar() << "' not defined!" << std::endl;
            util::logFatal(mLogStream);
        }

        return index;
    }

    /**
     * Provides a parser symbol by its name.
     * A fatal gothic log error is risen if the name specifies no parser symbol.
     * @param name : The name of the parser symbol.
     * @param callerDescription : A descrption of the caller calling this function. This descrption will be used for the error message.
     * @return : the parser symbol. Won't be nullptr.
     */
    static inline zCPar_Symbol* getSymbolWithChecks(const zSTRING& name, const std::string& callerDescription) {
        zCParser* parser; parser = zCParser::GetParser();
        int index = parser->GetIndex(name);

        if (index == -1) {
            std::stringstream mLogStream;
            mLogStream << callerDescription << ": '" << name.ToChar() << "' not defined!" << std::endl;
            util::logFatal(mLogStream);
        }

        return parser->GetSymbol(index);
    }

    static inline zCPar_Symbol* getSymbolWithChecks(int index, const std::string& callerDescription) {
        zCParser* parser; parser = zCParser::GetParser();
        auto* symbol = parser->GetSymbol(index);

        if (!symbol) {
            std::stringstream mLogStream;
            mLogStream << callerDescription << ": " << index << " is not an index to a parser symbol!" << std::endl;
            util::logFatal(mLogStream);
        }

        return symbol;
    }

	static std::string getGothicSystemDirectory();

    static void* __cdecl gothic2OperatorNew(size_t size) {
        XCALL(0x00565F50);
    }

	static void assertDIIRequirements(bool expression, std::string errorMessage);

	static void debug(std::stringstream& ss, Logger::LogLevel level = Logger::Info);

	static std::string trimFromRight(const std::string&);
	static void readString(std::istream& is, std::string& data);
    static void readzSTRING(std::istream& is, zSTRING& data);

    static void readAndTrim(std::istream& is, std::string& data);
    static void readAndTrim(std::istream& is, zSTRING& data);

    template<class T>
    static void readValue(std::istream& is, T& value) {
        is.read((char*)&value, sizeof(T));
    }

    template<class T>
    static void writeValue(std::ostream& os, const T& value) {
        os.write((char*)&value, sizeof(T));
    }

	static void writeString(std::ostream& os, const std::string& data);
    static void writezSTRING(std::ostream& os, const zSTRING& data);

	static void logInfo(std::stringstream& ss);
	static void logWarning(std::stringstream& ss);
	static void logFault(std::stringstream& ss);
	static void logFatal(std::stringstream& ss);

	static void logAlways(std::stringstream& ss);

	static void callDaedalusFunction_Int2(std::string functionName, int first, int second, bool isExternal);

    template<class T>
    static std::list<T*> create(zCListSort<T>* zclist) {

        std::list<T*> result;
        while (zclist != NULL) {
            auto* item = zclist->GetData();
            if (item != NULL) result.push_back(item);

            zclist = zclist->GetNext();
        }

        return result;
    }

private:
	util() {};
	~util() {};

	static HMODULE hModule;
	static std::stringstream mLogStream;

};