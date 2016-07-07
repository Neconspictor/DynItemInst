#pragma once

#include "Module.h"

class LoADebug : public Module
{
public:

	/**
	* Default constructor
	*/
	LoADebug();

	/**
	* Default virtual destructor.
	*/
	virtual ~LoADebug();

	/*! @copydoc Module::hookModule()
	*/
	virtual void hookModule() override;

	/*! @copydoc Module::unHookModule()
	*/
	virtual void unHookModule() override;

	static void __cdecl oCGameRenderHookNaked();
	static void __cdecl oCGameRenderHook();

};
