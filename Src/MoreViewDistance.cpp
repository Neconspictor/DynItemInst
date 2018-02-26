#include "MoreViewDistance.h"
#include <HookManager.h>
#include <Configuration.h>
#include "Util.h"

typedef void(__thiscall* ZCCameraSetFarClipZ)(void*, float);
ZCCameraSetFarClipZ zCCameraSetFarClipZ;



MoreViewDistance::MoreViewDistance()
{
	moduleDesc = "MoreViewDistance";
}

MoreViewDistance::~MoreViewDistance()
{
}

void MoreViewDistance::hookModule()
{
	zCCameraSetFarClipZ = (ZCCameraSetFarClipZ)ZCCAMERA_SET_CLIP_Z_ADDRESS;
	HookManager* hookManager = HookManager::getHookManager();

	hookManager->addFunctionHook((LPVOID*)&zCCameraSetFarClipZ, zCCameraSetFarClipZHookNaked, moduleDesc);
}

void MoreViewDistance::unHookModule()
{
	HookManager* hookManager = HookManager::getHookManager();
	hookManager->removeFunctionHook((LPVOID*)&zCCameraSetFarClipZ, zCCameraSetFarClipZHookNaked, moduleDesc);
}

_declspec(naked) void MoreViewDistance::zCCameraSetFarClipZHookNaked()
{
	_asm
	{
		LEGO_HOOKENGINE_PREAMBLE
		/*6 - 5 = 1 Byte for remaining opcode */
			nop
			/*finally hook function call*/
			jmp MoreViewDistance::zCCameraSetFarClipZHook
	}
}


void MoreViewDistance::zCCameraSetFarClipZHook(void* pThis, float value)
{
	float targetValue = 60000.0f;
	if (value != targetValue)
	{
		value *= Configuration::getFarClipZMultiplicator();
	}

	zCCameraSetFarClipZ(pThis, value);
}
