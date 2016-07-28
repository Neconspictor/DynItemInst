#include "LoA.h"
#include <HookManager.h>
#include <api/g2/zcparser.h>
#include "Util.h"

//.text:0073A030 public: void __thiscall oCNpc::EquipWeapon(class oCItem *) proc near
using OCNpcEquipWeapon = void(__thiscall*)(oCNpc*, oCItem*);
OCNpcEquipWeapon oCNpcEquipWeapon = (OCNpcEquipWeapon)0x0073A030;

LoA::LoA()
{
	moduleDesc = "LoA";
}

LoA::~LoA()
{
}

void LoA::hookModule()
{
	HookManager* manager = HookManager::getHookManager();
	manager->addFunctionHook((LPVOID*)&oCNpcEquipWeapon, oCNpcEquipWeaponHookNaked, moduleDesc);
}

void LoA::unHookModule()
{
	HookManager* manager = HookManager::getHookManager();
	manager->removeFunctionHook((LPVOID*)&oCNpcEquipWeapon, oCNpcEquipWeaponHookNaked, moduleDesc);
}

void LoA::oCNpcEquipWeaponHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*5 - 5 = 0 Byte for remaining opcode */
		/*finally hook function call*/
			jmp LoA::oCNpcEquipWeaponHook
	}
}


void LoA::oCNpcEquipWeaponHook(oCNpc* pThis, oCItem* item)
{
	//get callback function:
	zCParser* parser = zCParser::GetParser();
	zCPar_Symbol* callbackSymbol = parser->GetSymbol(zSTRING("OCNPC_EQUIPWEAPON_CALLBACK_NAME"));

	// call callback function
	util::callDaedalusFunction_Int2(callbackSymbol->content.data_pstring->ToChar(), (int)pThis, (int)item, false);

	// return to original function
	oCNpcEquipWeapon(pThis, item);
}