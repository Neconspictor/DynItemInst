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

#include "Module.h"
#include "Telekinesis.hpp"

class oCItem;
class zCParser;

/** 
 * A module for providing an interface to use this library in Daedalus script language.
 */
class General : public Module
{
public:

	/**
	 * Creates a new DaedalusExport object.
	 */
	General();

	/**
	 * Default virtual destructor.
	 */
	virtual ~General();

    /*! @copydoc Module::hookModule()
	 */
	virtual void hookModule() override;

	/*! @copydoc Module::unHookModule()
	 */
	virtual void unHookModule() override;

	/**
	 * Provides the version of the library.
	 */
	static float GetLibVersion();

public:

	/**
	 * The version of this library.
	 */
	static const float LIB_VERSION;
};