#pragma once
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