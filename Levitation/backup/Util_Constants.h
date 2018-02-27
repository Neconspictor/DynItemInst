#pragma once

#include <api/g2/zcvob.h>
#include <api/g2/ocnpc.h>

#define THISPTR_OFFSET(x) (((char *)this) + (x))

class zCPolygon;

class constants {
public:
	//.text:007A5180 public: void __thiscall zCPar_DataStack::Clear(void) proc near
	typedef void(__thiscall* ZCPar_StackClear)(void* pThis);
	static const ZCPar_StackClear zCPar_StackClear;

	//.text:007A4F80 public: void __thiscall zCPar_DataStack::Push(int) proc near
	typedef void(__thiscall* ZCPar_StackPush)(void* pThis, int);
	static const ZCPar_StackPush zCPar_StackPush;

	//.text:007A4B90 public: void __thiscall zCPar_Stack::PushInt(int) proc near
	typedef void(__thiscall* ZCPar_StackPushInt)(void* pThis, int);
	static const ZCPar_StackPushInt zCPar_StackPushInt;

	//.text:007A5070 public: int __thiscall zCPar_DataStack::Pop(void) proc near
	typedef int(__thiscall* ZCPar_DataStackPop)(void* pThis);
	static const ZCPar_DataStackPop zCPar_DataStackPop;

	//.text:007A1E10 public: void __thiscall zCPar_Symbol::GetStackPos(int &, int) proc near
	typedef void(__thiscall* ZCPar_SymbolGetStackPos)(void* pThis, int&, int);
	static const ZCPar_SymbolGetStackPos zCPar_SymbolGetStackPos;

	//.text:00791960 private: void __thiscall zCParser::DoStack(int) proc near
	typedef void(__thiscall* ZCParserDoStack)(void* pThis, int);
	static const ZCParserDoStack zCParserDoStack;


	//.text:0061D190 public: void __thiscall zCVob::SetPhysicsEnabled(int) proc near
	typedef void(__thiscall* ZCVobSetPhysicsEnabled)(zCVob*, int);
	static const  ZCVobSetPhysicsEnabled zCVobSetPhysicsEnabled;

	//.text:0061DA80 public: void __thiscall zCVob::BeginMovement(void) proc near
	typedef void(__thiscall* ZCVobBeginMovement)(zCVob*);
	static const ZCVobBeginMovement zCVobBeginMovement;

	//.text:0061E0D0 public: virtual void __thiscall zCVob::EndMovement(int) proc near
	typedef void(__thiscall* ZCVobEndMovement)(zCVob*, int);
	static const ZCVobEndMovement zCVobEndMovement;

	//.text:0061F255                 call    zCVob::CollectCollisionContext_Vobs(zTBBox3D const &,zCVob::zTCollisionContext &)
	//.text:0061E870 private: void __fastcall zCVob::CollectCollisionContext_Vobs(struct zTBBox3D const &, struct zCVob::zTCollisionContext &) proc near
	typedef void(__fastcall* ZCVobCollectCollisionContext_Vobs)(zCVob* pThis, zTBBox3D const &, int (&zTCollisionContext)[10]);
	static const ZCVobCollectCollisionContext_Vobs zCVobCollectCollisionContext_Vobs;

	//.text:00531110 public: void __fastcall zCBspBase::CollectVobsInBBox3D(class zCArray<class zCVob *> &, struct zTBBox3D const &)const proc near
	typedef void (__fastcall* ZCBspBaseCollectVobsInBBox3D)(void* pThis, zCArray<zCVob*>&, zTBBox3D const &);
	static const ZCBspBaseCollectVobsInBBox3D zCBspBaseCollectVobsInBBox3D;

	//.text:007D0A3A ___RTDynamicCast proc near 
	typedef void* (__cdecl* __RTDynamicCast)(int, int to, int from, int, void* source);
	static const __RTDynamicCast rTDynamicCast;

	//.text:005C8380 public: int __thiscall zCProgMeshProto::GetNumTri(void)const proc near
	typedef int (__thiscall* ZCProgMeshProtoGetNumTri)(void* pThis);
	static const ZCProgMeshProtoGetNumTri zCProgMeshProtoGetNumTri;

	//.text:005C83A0 public: class zCPolygon * __thiscall zCProgMeshProto::GetPolygon(int) proc near
	typedef void* (__thiscall* ZCProgMeshProtoGetPolygon)(void* pThis, int);
	static const ZCProgMeshProtoGetPolygon zCProgMeshProtoGetPolygon;

	//.text:0057DF20 public: virtual struct zTBBox3D __thiscall zCModel::GetBBox3D(void) proc near
	typedef zTBBox3D(__thiscall* ZCModelGetBBox3D)(void* pThis);
	static const ZCModelGetBBox3D zCModelGetBBox3D;

	//.text:00579170 public: void __thiscall zCModel::CalcModelBBox3DWorld(void) proc near
	typedef void(__thiscall* ZCModelCalcModelBBox3DWorld)(void* pThis);
	static const ZCModelCalcModelBBox3DWorld zCModelCalcModelBBox3DWorld;

	static int __fastcall zCBspBaseCollectPolysInBBox3D(void*, zTBBox3D const &, zCPolygon** &, int&);

	static inline bool areEqual(const float& a, const float& b)
	{
		if (abs(a - b) < EPSILON)
		{
			return true;
		}
		return false;
	};

	//.text:00490EA0 public: class zVEC3 & __thiscall zVEC3::Normalize(void) proc near
	typedef int(__thiscall* ZVEC3Normalize)(zVEC3*);
	static const ZVEC3Normalize zVEC3Normalize;

	//.text:006824D0 public: virtual void __thiscall oCNpc::ResetPos(class zVEC3 &) proc near
	typedef void(__thiscall* OCNpcResetPos)(oCNpc*, zVEC3 &);
	static const OCNpcResetPos oCNpcResetPos;

	//0061B720
	typedef void(__thiscall* RotateLocalY)(void*, float);
	static const RotateLocalY rotateLocalY;


	inline float getDistanceOfVectors(const zVEC3 & vec1, const zVEC3 & vec2) {
		float distance = abs(vec2.x - vec1.x + vec2.y - vec1.y + vec2.z - vec1.z);
		if (areEqual(distance, 0)) {
			return 0;
		}
		return distance;
	}

	inline bool zVEC3IsNull(const zVEC3 & vec)
	{
		if (areEqual(vec.x, 0)
			&& areEqual(vec.y, 0)
			&& areEqual(vec.z, 0))
		{
			return true;
		}
		return false;
	}


	//Module "DynItemInst"

	using  OCNpcSetWeaponMode2 = int(__thiscall*)(oCNpc*, int);
	static const OCNpcSetWeaponMode2 oCNpcSetWeaponMode2;

	using OCItemInitByScript = void(__thiscall*)(void* pThis, int, int);
	static const OCItemInitByScript oCItemInitByScript;

	using OCNpcEV_ForceRemoveWeapon = int(__thiscall*)(oCNpc* pThis, void*);
	static const OCNpcEV_ForceRemoveWeapon oCNpcEV_ForceRemoveWeapon;

	using OCNpcGetWeapon = oCItem*(__thiscall*)(oCNpc* pThis);
	static const OCNpcGetWeapon oCNpcGetWeapon;

	using OCNpcSetRightHand = int(__thiscall*)(oCNpc*, oCVob*);
	static const OCNpcSetRightHand oCNpcSetRightHand;

	using OCNpcGetWeaponMode = int(__thiscall*)(oCNpc*);
	static const OCNpcGetWeaponMode  oCNpcGetWeaponMode;

	using OCNpcIsMunitionAvailable = int(__thiscall*)(oCNpc*, oCItem*);
	static const OCNpcIsMunitionAvailable  oCNpcIsMunitionAvailable;

	using OCNpcSetLeftHand = int(__thiscall*)(oCNpc*, oCVob*);
	static const OCNpcSetLeftHand  oCNpcSetLeftHand;

	using OCNpcSetActiveSpellInfo = int(__thiscall*)(oCNpc*, int);
	static const OCNpcSetActiveSpellInfo  oCNpcSetActiveSpellInfo;

	using OCMag_BookDeRegister = int(__thiscall*)(oCMag_Book*, int);
	static const OCMag_BookDeRegister  oCMag_BookDeRegister;

	using OCSpell_GetSelectedSpell = oCSpell* (__thiscall*)(oCMag_Book*);
	static const OCSpell_GetSelectedSpell  oCSpell_GetSelectedSpell;

	using OCNpcGetSpellBook = oCMag_Book *(__thiscall*)(oCNpc*);
	static const OCNpcGetSpellBook  oCNpcGetSpellBook;

	using OCMag_BookSetFrontSpell = void(__thiscall*)(oCMag_Book*, int);
	static const OCMag_BookSetFrontSpell  oCMag_BookSetFrontSpell;

	using OCMag_DoOpen = void(__thiscall*)(oCMag_Book*);
	static const OCMag_DoOpen  oCMag_DoOpen;

	using OCMag_BookGetKeyByItem = int(__thiscall*)(oCMag_Book*, oCItem*);
	static const OCMag_BookGetKeyByItem  oCMag_BookGetKeyByItem;

	using OCMag_BookDeRegisterItem = int(__thiscall*)(oCMag_Book*, oCItem*);
	static const OCMag_BookDeRegisterItem  oCMag_BookDeRegisterItem;

	//static void(__thiscall* oCMag_BookKillSelectedSpell)(oCMag_Book*) = (void(__thiscall*)(oCMag_Book*))0x00477A90;
	using OCMag_BookKillSelectedSpell = void(__thiscall*)(oCMag_Book*);
	static const OCMag_BookKillSelectedSpell  oCMag_BookKillSelectedSpell;

	using OCMag_BookGetNoOfSpells = int(__thiscall*)(oCMag_Book*);
	static const OCMag_BookGetNoOfSpells  oCMag_BookGetNoOfSpells;

	using OCMag_GetNoOfSpellByKey = int(__thiscall*)(oCMag_Book*, int);
	static const OCMag_GetNoOfSpellByKey  oCMag_GetNoOfSpellByKey;

	using OCMag_BookGetSpellByKey = oCSpell* (__thiscall*)(oCMag_Book*, int);
	static const OCMag_BookGetSpellByKey  oCMag_BookGetSpellByKey;

	using OCMag_BookNextRegisterAt = void(__thiscall*)(oCMag_Book*, int);
	static const OCMag_BookNextRegisterAt  oCMag_BookNextRegisterAt;

	//static oCSpell*(__thiscall* oCMag_BookGetSelectedSpell)(oCMag_Book*) = (oCSpell*(__thiscall*)(oCMag_Book*))0x00477780;
	using OCMag_BookGetSelectedSpell = oCSpell*(__thiscall*)(oCMag_Book*);
	static const OCMag_BookGetSelectedSpell  oCMag_BookGetSelectedSpell;

	using OCSpellGetSpellID = int(__thiscall*)(oCSpell*);
	static const OCSpellGetSpellID  oCSpellGetSpellID;

	//.text:00549290 public: class zVEC3 __thiscall zMAT4::Rotate(class zVEC3 const &)const proc near
	using ZMAT4Rotate = zVEC3(__thiscall*)(zMAT4 const* mat, zVEC3 const &);
	static const ZMAT4Rotate zMAT4Rotate;

	//.text:00517780 public: void __thiscall zMAT4::PostRotateY(float) proc near
	using ZMAT4PostRotateY = void(__thiscall*)(zMAT4* mat, float);
	static const ZMAT4PostRotateY zMAT4PostRotateY;

	//.text:00516390 public: class zVEC3 __thiscall zMAT4::GetEulerAngles(void)const proc near
	using ZMAT4GetEulerAngles = zVEC3(__thiscall*)(zMAT4 const* mat);
	static const ZMAT4GetEulerAngles zMAT4GetEulerAngles;

	//zCVisualGetFileExtension
	//.text:004CA0C0 public: virtual class zSTRING const * __thiscall zCVisual::GetFileExtension(int) proc near
	using ZCVisualGetFileExtension = zSTRING const *(__thiscall*)(zCVisual*, int);
	static const ZCVisualGetFileExtension  zCVisualGetFileExtension;

	//.text:00515500 public: class zMAT4 __thiscall zMAT4::Inverse(void)const proc near
	using ZMAT4Inverse = zMAT4(__thiscall*)(void*);
	static const ZMAT4Inverse zMAT4Inverse;

	//.text:00515340 public: class zMAT4 __thiscall zMAT4::InverseLinTrafo(void)const proc near
	using ZMAT4InverseLinTrafo = zMAT4(__thiscall*)(void*);
	static const ZMAT4InverseLinTrafo zMAT4InverseLinTrafo;

	//.text:0050B450 public: void __thiscall zCLineCache::Line3D(class zVEC3 const &, class zVEC3 const &, struct zCOLOR, int) proc near
	using ZCLineCacheLine3D = void(__thiscall*)(void* pThis, zVEC3 const &, zVEC3 const &, zCOLOR, int);
	static const ZCLineCacheLine3D zCLineCacheLine3D;

	static void drawLine(zVEC3 const &, zVEC3 const &, zCOLOR, int);


	/*static oCSpell* (__thiscall* oCMag_BookGetSpellByKey)(oCMag_Book*, int) = (oCSpell* (__thiscall*)(oCMag_Book*, int))0x00479C60;

	static void(__thiscall* oCMag_BookDoClose)(oCMag_Book*) = (void(__thiscall*)(oCMag_Book*))0x004785E0;

	static int(__thiscall* oCMag_BookGetSelectedSpellNr)(oCMag_Book*) = (int(__thiscall*)(oCMag_Book*))0x004777D0;

	static void(__thiscall* oCNpcMakeSpellBook)(oCNpc*) = (void(__thiscall*)(oCNpc*))0x0075F040;

	static void(__thiscall* oCNpcDestroySpellBook)(oCNpc*) = (void(__thiscall*)(oCNpc*))0x0075F0C0;

	static void(__thiscall* oCNpcOpenSpellBook)(oCNpc*, int) = (void(__thiscall*)(oCNpc*, int))0x0073E990;

	static void(__thiscall* oCMag_BookStopSelectedSpell)(oCMag_Book*) = (void(__thiscall*)(oCMag_Book*))0x00477910;

	static void(__thiscall* oCMag_BookKillSelectedSpell)(oCMag_Book*) = (void(__thiscall*)(oCMag_Book*))0x00477A90;

	static void(__thiscall* oCItemRemoveEffect)(oCItem*) = (void(__thiscall*)(oCItem*))0x00712C00;*/




	//const int zPAR_TOK_PUSHINT      = 64; //0x40
	static const int ZCPAR_TOK_PUSHINT = 0x40;

	static const int OCITEM_FLAG_EQUIPPED = 0x40000000;

	static const float EPSILON;
};

struct GothicMemoryLocations
{
	struct zCPolygon
	{
		static const unsigned int Offset_VerticesArray = 0x00;
		static const unsigned int Offset_PolyNormalX = 0x0C;
		static const unsigned int Offset_PolyNormalY = 0x10;
		static const unsigned int Offset_PolyNormalZ = 0x14;
		static const unsigned int Offset_FeaturesArray = 0x2C;
		static const unsigned int Offset_NumPolyVertices = 0x30;
		static const unsigned int Offset_PolyFlags = 0x31;
		static const unsigned int Offset_Material = 0x18;
		static const unsigned int Offset_Lightmap = 0x1C;
	};

	struct zCMaterial
	{
		static const unsigned int Offset_Color = 0x38;
		static const unsigned int Offset_Texture = 0x34;
		static const unsigned int Offset_AlphaFunc = 0x74;
		static const unsigned int Offset_MatGroup = 0x40;
		static const unsigned int Offset_TexAniCtrl = 0x4C;

		static const unsigned int InitValues = 0x00564260;
		static const unsigned int Constructor = 0x00563E00;
		static const unsigned int Destructor = 0x00564070;
		static const unsigned int GetAniTexture = 0x0064BA20;

	};

	struct zCMorphMesh
	{
		static const unsigned int Offset_MorphMesh = 0x38;
		static const unsigned int Offset_TexAniState = 0x40;
		static const unsigned int AdvanceAnis = 0x005A6830;
		static const unsigned int CalcVertexPositions = 0x005A6480;
	};

	struct zCMeshSoftSkin
	{
		static const unsigned int Offset_NodeIndexList = 0x38C;
		static const unsigned int Offset_VertWeightStream = 0x0FC;
	};

	struct zCModelPrototype
	{
		static const unsigned int Offset_NodeList = 0x64;
		static const unsigned int Offset_MeshSoftSkinList = 0x64 + 16;
		static const unsigned int LoadModelASC = 0x0059E760;
		static const unsigned int ReadMeshAndTreeMSB = 0x00593180;

	};

	struct zCModelMeshLib
	{
		static const unsigned int Offset_NodeList = 0xB * 4;
		static const unsigned int Offset_MeshSoftSkinList = 0xC * 4;
	};

	struct zCModelTexAniState
	{
		static const unsigned int UpdateTexList = 0x00576DA0;
	};

	struct zCModel
	{
		static const unsigned int RenderNodeList = 0x00579560;
		static const unsigned int UpdateAttachedVobs = 0x00580900;
		static const unsigned int Offset_ModelProtoList = 0x64;
		static const unsigned int Offset_NodeList = 0x70;
		static const unsigned int Offset_MeshSoftSkinList = 0x7C;
		static const unsigned int Offset_MeshLibList = 0xBC;
		static const unsigned int Offset_AttachedVobList = 0x98;
		static const unsigned int AdvanceAnis = 0x0057CA90;
		static const unsigned int SIZE_AdvanceAnis = 0x119E;
		static const unsigned int RPL_AniQuality = 0x0057CCC1;
		static const unsigned int Offset_DrawHandVisualsOnly = 0x174;
		static const unsigned int Offset_ModelFatness = 0x128;
		static const unsigned int Offset_ModelScale = 0x12C;
		static const unsigned int Offset_Flags = 0x1F8;
		static const unsigned int Offset_DistanceModelToCamera = 0x120;
		static const unsigned int Offset_NumActiveAnis = 0x34;
		static const unsigned int Offset_AniChannels = 0x38;
		static const unsigned int GetVisualName = 0x0057DF60;
	};

	struct zCModelAni
	{
		static const unsigned int Offset_NumFrames = 0xD8;
		static const unsigned int Offset_Flags = 0xE0;
		static const unsigned int Mask_FlagIdle = 0x10;
	};


	struct zCProgMeshProto
	{
		static const unsigned int Offset_PositionList = 0x34;
		static const unsigned int Offset_NormalsList = 0x38;
		static const unsigned int Offset_Submeshes = 0xA4;
		static const unsigned int Offset_NumSubmeshes = 0xA8;
	};

	struct zCObject
	{
		static const unsigned int GetObjectName = 0x005A9CD0;
		static const unsigned int VTBL_ScalarDestructor = 0x12 / 4;
	};

	struct zCVisual
	{
		static const unsigned int VTBL_GetFileExtension = 17;
		static const unsigned int Destructor = 0x00606800;
	};
};

const int zMAT_GROUP_WATER = 5;
const int zMAT_GROUP_SNOW = 6;
class zCMaterial : public zCObject
{
public:
	int GetMatGroup()
	{
		return *(int *)((char*)this) + (GothicMemoryLocations::zCMaterial::Offset_MatGroup);
	}

	static int __thiscall IsPortalMaterial(void* pThis);

	bool hasNoCollision()
	{
		//.text:00565608                 mov     cl, [edi+70h]
		return THISPTR_OFFSET(0x70);
	}
};

zVEC3 operator*(const zMAT4& z_mat4, const zVEC3& vec);
zMAT4 operator*(float f, zMAT4 const& m);

//.text:00478210 class zMAT4 __cdecl operator*(class zMAT4 const &, class zMAT4 const &) proc near
zMAT4 __cdecl operator% (zMAT4 const&, zMAT4 const &);

//.text:00474370 class zVEC3 __cdecl operator*(class zMAT4 const &, class zVEC3 const &) proc near
zVEC3 __cdecl operator% (zMAT4 const&, zVEC3 const &);
