#pragma once

#include "Module.h"

class MoreViewDistance : public Module
{
public:

	/**
	* Default constructor
	*/
	MoreViewDistance();

	/**
	* Default virtual destructor.
	*/
	virtual ~MoreViewDistance();

	/*! @copydoc Module::hookModule()
	*/
	virtual void hookModule() override;

	/*! @copydoc Module::unHookModule()
	*/
	virtual void unHookModule() override;

private:

	static const int ZCCAMERA_SET_CLIP_Z_ADDRESS = 0x0054B200;

	static void zCCameraSetFarClipZHookNaked();
	static void __thiscall zCCameraSetFarClipZHook(void* pThis, float value);

};