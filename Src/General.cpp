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


#include <api/g2/zcparser.h>
#include <api/g2/oCItemExtended.h>
#include <ObjectManager.h>
#include <api/g2/oCObjectFactory.h>
#include <Logger.h>
#include <General.h>
#include <set>
#include <ocgameExtended.h>
#include <DII.h>
#include <Util.h>
#include <Levitation.h>
#include <zCPar_SymbolTable.h>
#include <api/g2/ocmob.h>


const float General::LIB_VERSION = 1.03f;

General::General() : Module("DaedalusExports")
{
	mModuleDesc = "DaedalusExports";
}

General::~General()
{
}

void General::hookModule()
{
}

void General::unHookModule()
{
}


float General::GetLibVersion()
{
	return LIB_VERSION;
}