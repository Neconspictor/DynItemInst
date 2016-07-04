#pragma once

#include "Module.h"
#include <list>
#include <vector>

class CustomNpcFocus : public Module
{
public:

	/**
	* Default constructor
	*/
	CustomNpcFocus();

	/**
	* Default virtual destructor.
	*/
	virtual ~CustomNpcFocus();

	/*! @copydoc Module::hookModule()
	*/
	virtual void hookModule() override;

	/*! @copydoc Module::unHookModule()
	*/
	virtual void unHookModule() override;

	static void oCNpcFocusInitFocusModesNaked();
	static void oCNpcFocusSetFocusModeNaked();
	static void __cdecl oCNpcFocusInitFocusModesHook();
	static void __cdecl oCNpcFocusSetFocusModeHook(int mode);
	static const int OCNPCFOCUS_CLASS_SIZE = 0x50;

private:

	// Holds number of predefined and custom focus modes
	static int MAX_NPC_FOCUS_MODES;

	// Holds the number of modes already defined by the engine
	static const int ENGINE_DEFINED_FOCUS_MODE_NUMBER = 6;

	static std::vector<void*> customFocuses;

	//.data:00AB0738 public: static class oCNpcFocus * oCNpcFocus::focus dd ?
	static const int oCNpcFocus_FOCUS_ADDRESS = 0x00AB0738;
};
