#pragma once

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