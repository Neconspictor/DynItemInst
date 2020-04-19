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
#include <api/g2/macros.h>
#include <api/g2/zstring.h>
#include <api/g2/zcpar_symbol.h>

namespace g2ext_extended
{
	/** Insert description */
	typedef struct
	{
		zCPar_Symbol*	preAllocatedSymbols; //ecx
		int				nextPreAllocated;    // ecx + 0x4
											 /*
											 zCPar_Smybol* symbolList; //ecx + 0x8 (0x007A3EED)
											 int unknown;	//ecx + 0xB
											 int numInList;  //ecx + 0x10
											 int unknown;    // ecx+ 0x14; maybe zSTRING* symbolNameList ?

											 */

		int IsIn(zSTRING const &)
		{
			XCALL(0x007A3960);
		};

		int GetIndex(zSTRING const &)
		{
			XCALL(0x007A3B60);
		};

		zCPar_Symbol* GetSymbol(zSTRING const &)
		{
			XCALL(0x007A3E40);
		}

		void Insert(zCPar_Symbol*)
		{
			XCALL(0x007A3F00);
		};

		void SetSize(int)
		{
			XCALL(0x007A4430);
		};

		//.text:007A3EE0 public: class zCPar_Symbol * __thiscall zCPar_SymbolTable::GetSymbol(int) proc near
		zCPar_Symbol* GetSymbol(int)
		{
			XCALL(0x007A3EE0);
		}

	} zCPar_SymbolTable;
}