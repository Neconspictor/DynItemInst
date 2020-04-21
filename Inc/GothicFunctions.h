#pragma once

#include "GothicMemoryLocations.h"

struct zVEC3;
struct zMAT4;
class oCItem;
class oCNpc;
class zCModel;
struct zTSpatialState;
class oCItemContainer;
class zCVob;


using ZCVobDoFrameActivity = void(__thiscall*)(void*); extern ZCVobDoFrameActivity zCVobDoFrameActivity;
using ZCVobSetPhysicsEnabled = void(__thiscall*)(void*, int); extern ZCVobSetPhysicsEnabled zCVobSetPhysicsEnabled;

using OCGamePause = void(__thiscall*)(void*, int); extern OCGamePause oCGamePause;
using OCGameUnpause = void(__thiscall*)(void*); extern OCGameUnpause oCGameUnpause;

typedef void(__thiscall* ZCVobSetCollDet)(void*, int); ZCVobSetCollDet zCVobSetCollDet;
typedef int(__thiscall* ZCAIPlayerCheckFloorSliding)(void*); ZCAIPlayerCheckFloorSliding zCAIPlayerCheckFloorSliding;
typedef void(__thiscall* DoSurfaceAlignment)(void*); DoSurfaceAlignment doSurfaceAlignment;
typedef void(__thiscall* ZCVobCheckAndResolveCollisions)(void*); ZCVobCheckAndResolveCollisions zCVobCheckAndResolveCollisions;
typedef void(__thiscall* ZCTriggerOnTouch)(void*, zCVob*); ZCTriggerOnTouch zCTriggerOnTouch;
typedef int(__thiscall* ZCVobHasEnoughSpace)(void*, zVEC3&); ZCVobHasEnoughSpace zCVobHasEnoughSpace;
typedef zVEC3(__thiscall* ZCVobGetVelocity)(void*); ZCVobGetVelocity zCVobGetVelocity;

typedef void(__thiscall* ZCVobBeginMovement)(void* pThis);
ZCVobBeginMovement  zCVobBeginMovement = (ZCVobBeginMovement)0x0061DA80;

//.text:007380E0 public: int __thiscall oCNpc::IsMovLock(void) proc near
typedef int(__thiscall* OCNpcIsMovLock)(oCNpc*);
OCNpcIsMovLock oCNpcIsMovLock = (OCNpcIsMovLock)0x007380E0;

//.text:00579170 public: void __thiscall zCModel::CalcModelBBox3DWorld(void) proc near
typedef void(__thiscall* ZCModelCalcModelBBox3DWorld)(zCModel*);
ZCModelCalcModelBBox3DWorld zCModelCalcModelBBox3DWorld = (ZCModelCalcModelBBox3DWorld)0x00579170;

//.text:00511320 public: int __thiscall zCAIPlayer::CheckEnoughSpaceMoveDir(class zVEC3 const &, int) proc near
typedef int(__thiscall* ZCAIPlayerCheckEnoughSpaceMoveDir)(void*, zVEC3 const&, int); ZCAIPlayerCheckEnoughSpaceMoveDir zCAIPlayerCheckEnoughSpaceMoveDir;

//.text:00699F60 ; protected: int __thiscall oCAIHuman::PC_ActionMove(int)
typedef int(__thiscall* OCAIHumanPC_ActionMove)(void* pThis, int); 
  OCAIHumanPC_ActionMove oCAIHumanPC_ActionMove = (OCAIHumanPC_ActionMove)0x00699F60;




typedef  int(__fastcall* ZCCollObjectCharacterCalcSlideVector)(void* pThis, zVEC3 const& vec1, zVEC3 const& vec2, zVEC3& vec3, zVEC3& vec4, float& floatValue);
ZCCollObjectCharacterCalcSlideVector zCCollObjectCharacterCalcSlideVector;

//.text:00487C50; public: class zMAT4 & __thiscall zMAT4::SetTranslation(class zVEC3 const &)
typedef zMAT4& (__thiscall* ZMAT4SetTranslation)(zMAT4* pThis, zVEC3 const& translation);
ZMAT4SetTranslation zMAT4SetTranslation;

typedef  void(__thiscall* ZCVobCalcGroundPoly)(void* pThis);
ZCVobCalcGroundPoly zCVobCalcGroundPoly;


typedef  void(__thiscall* ZCVobUpdatePhysics)(void* pThis);
ZCVobUpdatePhysics zCVobUpdatePhysics;



//.text:0061E0D0 ; public: virtual void __thiscall zCVob::EndMovement(int)
typedef void(__thiscall* ZCVobEndMovement)(void* pThis, int arg0);
ZCVobEndMovement zCVobEndMovement;

//.text:005501F0; int __cdecl TestStatic_Char_Char(float, class zCCollisionObject *, class zMAT4 const &, class zCCollisionObject *, class zMAT4 const &, class zCCollisionReport * *)
typedef int(__cdecl* TestStatic_Char_Char)(float, void* collisionObject1, zMAT4 const& mat1, void* collisionObject2, zMAT4 const& mat2, void* collisionReportPtr);
TestStatic_Char_Char testStatic_Char_Char;

typedef int(__fastcall* ZCCollObjectCharacterTestSoftCollisions)(void* pThis, zVEC3 const& vec1, zVEC3& vec2, zVEC3& vec3, zVEC3& vec4);
ZCCollObjectCharacterTestSoftCollisions zCCollObjectCharacterTestSoftCollisions;


//.text:00551150 ; private: int __fastcall zCCollObjectCharacter::TestHardCollisions(class zVEC3 const &, class zVEC3 const &, class zVEC3 &, struct zCCollObjectCharacter::zTSpatialState &, class zVEC3 &)
typedef int(__fastcall* ZCCollObjectCharacterTestHardCollisions)(void* pThis, zVEC3 const& vec1, zVEC3 const& vec2, zVEC3& vec3, zTSpatialState* zTSpatialState, zVEC3& vec4);
ZCCollObjectCharacterTestHardCollisions zCCollObjectCharacterTestHardCollisions;

typedef void(__fastcall* ZCCollObjectCharacterDetectCollisionsSelf)(void* pThis, void* collisionObjectArray, void* collisionReports);
ZCCollObjectCharacterDetectCollisionsSelf zCCollObjectCharacterDetectCollisionsSelf;

//.text:0050DC40 ; private: void __thiscall zCAIPlayer::CheckPhysics(void)
typedef void(__thiscall* ZCAIPlayerCheckPhysics)(void* pThis);
ZCAIPlayerCheckPhysics zCAIPlayerCheckPhysics;

//.text:00550E20 ; private: void __fastcall zCCollObjectCharacter::FindFloorWaterCeiling(class zVEC3 const &, struct zCCollObjectCharacter::zTSpatialState &)
typedef void(__fastcall* ZCCollObjectCharacterFindFloorWaterCeiling)(void* pThis, zVEC3 const& vec1, void* zTSpatialState);
ZCCollObjectCharacterFindFloorWaterCeiling zCCollObjectCharacterFindFloorWaterCeiling;

//.text:00706B60 ; protected: virtual void __thiscall oCItemContainer::DrawCategory(void)
using OCItemContainerDrawCategory = void(__thiscall*)(void* pThis);
 OCItemContainerDrawCategory oCItemContainerDrawCategory = (OCItemContainerDrawCategory)0x00706B60;

//.text:007092C0 ; public: virtual class oCItem * __thiscall oCItemContainer::GetSelectedItem(void)
using OCItemContainerGetSelectedItem = oCItem * (__thiscall*)(oCItemContainer * pThis);
 OCItemContainerGetSelectedItem oCItemContainerGetSelectedItem = (OCItemContainerGetSelectedItem)0x007092C0;

//.text:00707BA4 ; naked function
using OCItemContainerDrawGetItem = void (*)();
 OCItemContainerDrawGetItem oCItemContainerDrawGetItem = (OCItemContainerDrawGetItem)0x00707BA4;

//.text:00709740 ; void __thiscall oCItemContainer::NextItem(oCItemContainer *__hidden this)
using OCItemContainerNextItem = void(__thiscall*)(oCItemContainer * pThis);
 OCItemContainerNextItem oCItemContainerNextItem = (OCItemContainerNextItem)0x00709740;

//.text:007076B0 ; void __thiscall oCItemContainer::Draw(oCItemContainer *this)
using OCItemContainerDraw = void(__thiscall*)(oCItemContainer * pThis);
 OCItemContainerDraw oCItemContainerDraw = (OCItemContainerDraw)0x007076B0;

//.text:00705B80 ; int __cdecl oCItemCompareFunction(int,int)
using OCItemCompareFunction = int(__cdecl*)(oCItem * a, oCItem * b);
 OCItemCompareFunction oCItemCompareFunction = (OCItemCompareFunction)0x00705B80;






 //.text:00490EA0 public: class zVEC3 & __thiscall zVEC3::Normalize(void) proc near
 typedef int(__thiscall* ZVEC3Normalize)(zVEC3*);
 ZVEC3Normalize zVEC3Normalize;

 //.text:006824D0 public: virtual void __thiscall oCNpc::ResetPos(class zVEC3 &) proc near
 typedef void(__thiscall* OCNpcResetPos)(oCNpc*, zVEC3&);
 OCNpcResetPos oCNpcResetPos;

 //.text:0061E0D0 public: virtual void __thiscall zCVob::EndMovement(int) proc near
 //typedef void(__thiscall* ZCVobEndMovement)(zCVob*, int);
 //extern ZCVobEndMovement zCVobEndMovement;

 //0061B720
 typedef void(__thiscall* RotateLocalY)(void*, float);
 RotateLocalY rotateLocalY;


 ZVEC3Normalize zVEC3Normalize = (ZVEC3Normalize)0x00490EA0;
 OCNpcResetPos oCNpcResetPos = (OCNpcResetPos)0x006824D0;
 //.text:0061E0D0 public: virtual void __thiscall zCVob::EndMovement(int) proc near
 //ZCVobEndMovement zCVobEndMovement = (ZCVobEndMovement)0x0061E0D0;
 RotateLocalY rotateLocalY = (RotateLocalY)0x0061B720;