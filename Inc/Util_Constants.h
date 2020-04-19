#pragma once
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

namespace constants {

	//.text:007A5180 public: void __thiscall zCPar_DataStack::Clear(void) proc near
	typedef void(__thiscall* ZCPar_StackClear)(void* pThis);
	ZCPar_StackClear zCPar_StackClear = (ZCPar_StackClear)0x007A5180;

	//.text:007A4F80 public: void __thiscall zCPar_DataStack::Push(int) proc near
	typedef void(__thiscall* ZCPar_StackPush)(void* pThis, int);
	ZCPar_StackPush zCPar_StackPush = (ZCPar_StackPush)0x007A4F80;

	//.text:007A4B90 public: void __thiscall zCPar_Stack::PushInt(int) proc near
	typedef void(__thiscall* ZCPar_StackPushInt)(void* pThis, int);
	ZCPar_StackPushInt zCPar_StackPushInt = (ZCPar_StackPushInt)0x007A4B90;

	//.text:007A5070 public: int __thiscall zCPar_DataStack::Pop(void) proc near
	typedef int(__thiscall* ZCPar_DataStackPop)(void* pThis);
	ZCPar_DataStackPop zCPar_DataStackPop = (ZCPar_DataStackPop)0x007A5070;

	//.text:007A1E10 public: void __thiscall zCPar_Symbol::GetStackPos(int &, int) proc near
	typedef void(__thiscall* ZCPar_SymbolGetStackPos)(void* pThis, int&, int);
	ZCPar_SymbolGetStackPos zCPar_SymbolGetStackPos = (ZCPar_SymbolGetStackPos)0x007A1E10;

	//.text:00791960 private: void __thiscall zCParser::DoStack(int) proc near
	typedef void(__thiscall* ZCParserDoStack)(void* pThis, int);
	ZCParserDoStack zCParserDoStack = (ZCParserDoStack)0x00791960;

	//const int zPAR_TOK_PUSHINT      = 64; //0x40
	static int ZCPAR_TOK_PUSHINT = 0x40;

};