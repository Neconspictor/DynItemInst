#include "DynamicParsing.h"
#include <api/g2/zcparser.h>
#include <Util.h>

DynamicParsing::DynamicParsing()
{
	moduleDesc = "DynamicParsing";
}

DynamicParsing::~DynamicParsing()
{
}

void DynamicParsing::hookModule()
{
	logStream << "DynamicParsing::hookModule: try to parse custom source file..." << std::endl;
	util::debug(&logStream);
	bool* enableParsing = (bool*)zCParserEnableParsingAddress;
	*enableParsing = true;

	zCParser* parser = zCParser::GetParser();
	parser->Parse(zSTRING("D:\\Program Files\\JoWood\\Gothic II Gold\\System\\LoA\\MyTestFile.d"));
	parser->CreatePCode();
	*enableParsing = false;
	logStream << "DynamicParsing::hookModule: finished." << std::endl;
	util::debug(&logStream);
}

void DynamicParsing::unHookModule()
{
}