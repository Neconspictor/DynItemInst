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

/////////////////////////////////////////////////////////////////////////////**/

#ifndef __LOGGER_H__
#define __LOGGER_H__
#include <sstream>

/**
 * Provides basic logging functionality to different streams. Currently supported:
 * Files, console and zSpy.
 */
class Logger
{
public:

	/**
	 * Specifies several levels of logging.
	 */
	enum LogLevel
	{
		/**
		 * Message will be marked as information.
		 */
		Info = 1,

		/**
		 * Message will be marked as warning. Only if zSpy is activated: 
		 * The message will be displayed in greenish color.
		 */
		Warning = 2,

		/**
		 * Message will be marked as Fault. Only if zSpy is activated: 
		 * The message will be displayed in reddish color and a message box 
		 * will interrupt the game with the message as content. 
		 * Use it only when needed.
		 */
		Fault = 3,

		/**
		 * Message will be marked as Fault. Only if zSpy is activated: 
		 * The message will be displayed in red and a message box will interrupt 
		 * the game with the message as content. When the user closes the message box the 
		 * game will be shutdown. Use it only when really needed.
		 */
		Fatal = 4
	};

	/**
	 * Default destructor.
	 */
	~Logger();

	/**
	 * \return The current instance of this class.
	 */
	static Logger* getLogger();

	/**
	 * Releases all memory allocated by this class.
	 */
	static void release();

	/**
	 * Writes the given message 'message' to the log file.
	 * \param message The message to be logged.
	 */
	void writeToFile(std::string message);

	/**
	 * Writes the given message 'message' to zSpy with and log level 'level'.
	 * \param level The logging level for this log.
	 * \param message The message to be logged.
	 */
	void writeTozSpy(LogLevel level, std::string message);

	/**
	 * Logs the content of the specified stringstream 'stream' with an log level 'level'. 
	 * \param level The logging level for this log
	 * \param stream The stream to extract the log message from
	 * \param toFile Should the log message be written to file?
	 * \param tozSpy Should the log message be written to zSpy?
	 * \param toConsole Should the log message be written to console (std::cout)?
	 */
	void log(LogLevel level, std::stringstream* stream, bool toFile = false, bool tozSpy = true, bool toConsole = false);
public:

	/**
	 * The name of the log file.
	 */
	static const std::string logFileName;

private:
	static Logger* instance;

	/**
	 * The address of the gothic 2 function zERROR::Report(int, int, zSTRING const &, signed char, UINT, int, char*, char*)
	 */
	static const int ZERROR_REPORT_ADDRESS = 0x0044C8D0;

	/**
	 * Private constructor because this class is a singleton.
	 */
	Logger();
};

#endif __LOGGER_H__