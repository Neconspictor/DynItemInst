#pragma once
#include "Module.h"

//.data:00AB6284 private: static int zCParser::enableParsing dd ?

class DynamicParsing : public Module
{
public:

	static const int zCParserEnableParsingAddress = 0x00AB6284;

	DynamicParsing();
	virtual ~DynamicParsing();

	/*! @copydoc Module::hookModule()
	*/
	virtual void hookModule() override;

	/*! @copydoc Module::unHookModule()
	*/
	virtual void unHookModule() override;

};