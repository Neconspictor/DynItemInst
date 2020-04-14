#pragma once

#include "CustomNpcFocus.h"
#include <HookManager.h>
#include <api/g2/zcparser.h>
#include "DII.h"
#include <Logger.h>
#include "Util.h"

//.text:006BEA80 public: static void __cdecl oCNpcFocus::InitFocusModes(void) proc near
typedef void(__cdecl* OCNpcFocusInitFocusModes)();
OCNpcFocusInitFocusModes oCNpcFocusInitFocusModes = (OCNpcFocusInitFocusModes)0x006BEA80;

//.text:006BEDD0 public: __thiscall oCNpcFocus::oCNpcFocus(void) proc near
typedef void(__thiscall* OCNpcFocus)(void* pThis);
OCNpcFocus oCNpcFocus = (OCNpcFocus)0x006BEDD0;

//.text:006BEE70 public: void __thiscall oCNpcFocus::Init(int) proc near
typedef void(__thiscall* OCNpcFocusInit)(void* pThis, int parserSymbolIndex);
OCNpcFocusInit oCNpcFocusInit = (OCNpcFocusInit)0x006BEE70;

//.text:006BEC20 public: static void __cdecl oCNpcFocus::SetFocusMode(int) proc near
typedef void(__cdecl* OCNpcFocusSetFocusMode)(int mode);
OCNpcFocusSetFocusMode oCNpcFocusSetFocusMode = (OCNpcFocusSetFocusMode)0x006BEC20;


int CustomNpcFocus::MAX_NPC_FOCUS_MODES = 0;
std::vector<void*> CustomNpcFocus::customFocuses;


CustomNpcFocus::CustomNpcFocus()
{
	zCParser* parser = zCParser::GetParser();

	// Get the number of defined focus modes
	int index = parser->GetIndex("MAX_NPC_FOCUS_MODES");
	zCPar_Symbol* symbol = parser->GetSymbol(index);
	MAX_NPC_FOCUS_MODES = symbol->content.data_int;

	// There is a minium amount of focus modes
	if (MAX_NPC_FOCUS_MODES < ENGINE_DEFINED_FOCUS_MODE_NUMBER)
	{
		MAX_NPC_FOCUS_MODES = ENGINE_DEFINED_FOCUS_MODE_NUMBER;
	}

	moduleDesc = "CustomNpcFocus";
}

CustomNpcFocus::~CustomNpcFocus()
{
}

void CustomNpcFocus::hookModule()
{
	HookManager* hookManager = HookManager::getHookManager();
	hookManager->addFunctionHook((LPVOID*)&oCNpcFocusInitFocusModes, oCNpcFocusInitFocusModesNaked, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCNpcFocusSetFocusMode, oCNpcFocusSetFocusModeNaked, moduleDesc);

	zCParser* parser = zCParser::GetParser();

	//get index begin of the defined focus mode array
	int indexBegin = zCParser::GetParser()->GetIndex(zSTRING("NPCFOCUSMODES"));

	logStream << "ENGINE_DEFINED_FOCUS_MODE_NUMBER: " << ENGINE_DEFINED_FOCUS_MODE_NUMBER << std::endl;
	logStream << "indexBegin: " << indexBegin << std::endl;
	Logger::getLogger()->log(Logger::Info, &logStream);

	zCPar_Symbol* symbol = zCParser::GetParser()->GetSymbol(indexBegin);
	zSTRING* names = (zSTRING*)symbol->content.data_ptr;

	for (int i = ENGINE_DEFINED_FOCUS_MODE_NUMBER; i < MAX_NPC_FOCUS_MODES; ++i)
	{
		zSTRING symbolName = names[i].Upper().ToChar();
		logStream << "oCNpcFocus to add: " << symbolName.ToChar() << std::endl;
		Logger::getLogger()->log(Logger::Info, &logStream);

		int index = parser->GetIndex(symbolName);
		void* npcFocus = malloc(OCNPCFOCUS_CLASS_SIZE);
		oCNpcFocus(npcFocus);
		oCNpcFocusInit(npcFocus, index);
		customFocuses.push_back(npcFocus);
	}
}

void CustomNpcFocus::unHookModule()
{
	HookManager* hookManager = HookManager::getHookManager();
	hookManager->removeFunctionHook((LPVOID*)&oCNpcFocusInitFocusModes, oCNpcFocusInitFocusModesNaked, moduleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCNpcFocusSetFocusMode, oCNpcFocusSetFocusModeNaked, moduleDesc);
}


_declspec(naked) void CustomNpcFocus::oCNpcFocusInitFocusModesNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*7 - 5 = 2 Bytes for remaining opcode */
			nop
			nop
			/*finally hook function call*/
			jmp CustomNpcFocus::oCNpcFocusInitFocusModesHook
	}
}

_declspec(naked) void CustomNpcFocus::oCNpcFocusSetFocusModeNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*7 - 5 = 2 Bytes for remaining opcode */
			nop
			nop
			/*finally hook function call*/
			jmp CustomNpcFocus::oCNpcFocusSetFocusModeHook
	}
}

void __cdecl CustomNpcFocus::oCNpcFocusInitFocusModesHook()
{
	// finally call original function
	oCNpcFocusInitFocusModes();
}

void __cdecl CustomNpcFocus::oCNpcFocusSetFocusModeHook(int mode)
{
	// init custom focus modes
	if (mode >= ENGINE_DEFINED_FOCUS_MODE_NUMBER)
	{
		void* focus = customFocuses.at(mode - ENGINE_DEFINED_FOCUS_MODE_NUMBER);
		void** currentFocus = (void**)oCNpcFocus_FOCUS_ADDRESS;
		*currentFocus = focus;
	} else
	{
		oCNpcFocusSetFocusMode(mode);
	}
	
}