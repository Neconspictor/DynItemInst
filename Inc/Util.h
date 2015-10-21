/*////////////////////////////////////////////////////////////////////////////

This file is part of DynItemInst.

Copyright � 2015 David Goeth

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

#ifndef __UTIL_H__
#define __UTIL_H__
#define SAFE_DELETE(pointer) util::SafeDelete(reinterpret_cast<void**>(&pointer));

#include <vector>

/**
* Contains functions that are useful for several classes.
*/
namespace util {

	/**
	 * Checks if the specified path is an directory and exists.
	 * \param path The directory path to check.
	 * \return Exists the specified directory path?
	 */
	bool existsDir(const std::string& path);

	/**
	 * Creates the provied folder structure.
	 * \param path The directory path to create
	 * \return Was the path creation successful?
	 */
	bool makePath(const std::string& path);

	/**
	 * Splits the given text 'text' on the base of the separation character 'sep'
	 * \param tokens A vector which will be used to store all substrings that were created 
	 * by seperating the given text.
	 * \param text The text that should be splitted.
	 * \param sep Always this character was found in the text the latter will be splitted at the
	 * current position.
	 */
	void split(std::vector<std::string> &tokens, const std::string &text, char sep);

	/**
	 * Copies all files from a specified source path which additionally begin with the pattern
	 * 'pattern' and moves the result to the destination folder 'dest'.
	 * \param source The directory to copy from
	 * \param des The directory to copy to
	 * \param pattern Only files, which names begin with this pattern, will be treated.
	 */
	void copyContentTo(std::string source, std::string dest, std::string pattern);

	/**
	 * Deletes all files from the folder 'folder' which begin with the pattern 'pattern'.
	 * \param folder The directory to delete the files from.
	 * \param pattern Only files, which names begin with this pattern, will be treated.
	 */
	void deleteAllFiles(std::string folder, std::string pattern);

	/**
	 * Copies a file and moves its copy to a specified destination.
	 * \param from The file to be copied.
	 * \param to The file path the file should be copied to.
	 */
	void copyFileTo(std::string from, std::string to);

	/**
	 * Takes the address of an pointer and deletes the pointer and sets its content to NULL.
	 * \param address The address of the pointer
	 */
	void SafeDelete(void** address);

	/**
	 * Provides the pth to the current working directory of this application.
	 * \return The current working directory
	 */
	std::string getCurrentWorkingDir(); 
};

#endif __UTIL_H__