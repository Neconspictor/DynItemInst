#include <api/g2/zcpar_symbol.h>
#include <Util.h>


zCPar_Symbol* zCPar_Symbol::create()
{
	typedef DWORD(__thiscall* ZCPAR_SYMBOL_CONSTRUCTOR)(void* thizz);
	static ZCPAR_SYMBOL_CONSTRUCTOR zCPar_SymbolConstructor = (ZCPAR_SYMBOL_CONSTRUCTOR)0x007A1690;

	// The symbol will be managed by the Gothic 2 exe and thus the appropriate new operator has to be called!
	void* memory = util::gothic2OperatorNew(0x3C); //sizeof(zCPar_Symbol) = 0x3C (-> see gothic 2 exe, address 0x0078DD02)
	ZeroMemory(memory, 0x3C);
	zCPar_SymbolConstructor(memory);
	return (zCPar_Symbol*)memory;
}