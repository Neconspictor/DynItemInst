const string NEC_relativeLibraryPath = ".\neclib\neclib.dll";

// flags for lib config
const int NEC_DII = 1 << 0;
const int NEC_LEVITATION = 1 << 1;
const int NEC_TELEKINESIS = 1 << 2;
const int NEC_ALL = NEC_DII | NEC_LEVITATION | NEC_TELEKINESIS;

// Keeps track of initialized modules
const int NEC_Init_Modules = 0;


// *****************************************************************
// Provides the version number of the DynItemInst_Ikarus library.
// Important: The version number is a float digit, that is returned
// as an integer. Therefore you should use this function along with
// e.g. the float package of Ikarus.
// *****************************************************************
FUNC INT NEC_GetLibVersion() {
    var int adr;
    adr = GetProcAddress (LoadLibrary (NEC_relativeLibraryPath), "GetLibVersion");

    //library couldn't be found?
    if (!adr) {
		MEM_Error(ConcatStrings("neclib: NEC_GetLibVersion(): Library couldn't be loaded: ", NEC_relativeLibraryPath));
        return 0;
    };

    var int ret;
	CALL_RetValIsFloat();
	CALL_PutRetValTo(_@(ret));
	CALL__cdecl(adr);
    return +ret;
};

// ****************************************************************
// Returns the expected version of the used DynItemInst_Ikarus.dll.
// The return value will be float stored in an integer.
// ****************************************************************
FUNC INT _NEC_GetExpectedLibVersion() {
    //expected lib version: 1.03
    var int integral;
    var int fraction;

    integral = mkf(1);
    fraction = divf(4, 100);
    return addf(integral, fraction);
};

//.text:006BCEF0 ; public: int __thiscall oCNpc::CanTalk(void)
func int oCNpcCanTalk(var C_NPC npc) {
    const int adr = 7065328;
    var int ret;
	CALL_PutRetValTo(_@(ret));
	CALL__thiscall(_@(npc), adr);
    return +ret;
};

//.text:00602930 ; public: void __thiscall zCVob::SetSleeping(int)
func void zCVobSetSleeping(var int czvobPtr, var int setToSleep) {
	const int adr = 6302000;
	CALL_IntParam(setToSleep);
	CALL__thiscall(czvobPtr, adr);
};


//.text:0061D190 ; public: void __thiscall zCVob::SetPhysicsEnabled(int)
func void zCVobSetPhysicsEnabled(var int czvobPtr, var int enablePhysics) {
	const int adr = 6410640;
	CALL_IntParam(enablePhysics);
	CALL__thiscall(czvobPtr, adr);
};


//.text:0061BB70 ; void __thiscall zCVob::SetPositionWorld(zCVob *this, const struct zVEC3 *)
func void zCVobSetPositionWorld(var int czvobPtr, var int zvec3Ptr) {
	const int adr = 6404976;
	CALL_IntParam(zvec3Ptr);
	CALL__thiscall(czvobPtr, adr);
};



 //************************************************
// Less restrict external test
// by Sektenspinner: https://forum.worldofplayers.de/forum/threads/969446-Skriptpaket-Ikarus-3/page9?p=16955344#post16955344
//************************************************

const int _ExternalAcceptVobsCount = 0;
const int _oCNpcTypeInfo = 8983472;  //0x8913B0
const int _zCVobTypeInfo = 8970208;  //0x88DFE0

func void ExternalAcceptVobs() {
    if (!_ExternalAcceptVobsCount) {
        const int once = 0;
    
        var int ptr; ptr = 7188726; //0x6DB0F6
        if (!once) { MemoryProtectionOverride(ptr, 4); };
        MEM_WriteInt(ptr, _zCVobTypeInfo); //0x88DFE0
        
        ptr = 7188773; //0x6DB125
        if (!once) { MemoryProtectionOverride(ptr, 4); };
        MEM_WriteInt(ptr, _zCVobTypeInfo); //0x88DFE0
        
        once = true;
    };
    
    _ExternalAcceptVobsCount += 1;
};

func void ExternalDenyVobs() {
    _ExternalAcceptVobsCount -= 1;
    
    if (!_ExternalAcceptVobsCount) {
        var int ptr; ptr = 7188726; //0x6DB0F6
        MEM_WriteInt(ptr, _oCNpcTypeInfo); //0x88DFE0
        
        ptr = 7188773; //0x6DB125
        MEM_WriteInt(ptr, _oCNpcTypeInfo); //0x88DFE0
    };
};