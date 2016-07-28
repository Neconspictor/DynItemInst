#pragma once
#include "Module.h"
#include "oCItemExtended.h"
#include <api/g2/ocnpc.h>

class LoA : public Module
{
public:

	/**
	* Default constructor
	*/
	LoA();

	/**
	* Default virtual destructor.
	*/
	virtual ~LoA();

	/*! @copydoc Module::hookModule()
	*/
	virtual void hookModule() override;

	/*! @copydoc Module::unHookModule()
	*/
	virtual void unHookModule() override;

	static void __cdecl oCNpcEquipWeaponHookNaked();
	static void __thiscall oCNpcEquipWeaponHook(oCNpc*, oCItem*);
};