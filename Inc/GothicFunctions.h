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

using ZCVobSetCollDet = void(__thiscall*)(void*, int); extern ZCVobSetCollDet zCVobSetCollDet;
using ZCAIPlayerCheckFloorSliding = int(__thiscall*)(void*); extern ZCAIPlayerCheckFloorSliding zCAIPlayerCheckFloorSliding;

using DoSurfaceAlignment = void(__thiscall*)(void*); extern DoSurfaceAlignment doSurfaceAlignment;
using ZCVobCheckAndResolveCollisions = void(__thiscall*)(void*); extern ZCVobCheckAndResolveCollisions zCVobCheckAndResolveCollisions;

using ZCTriggerOnTouch = void(__thiscall*)(void*, zCVob*); extern ZCTriggerOnTouch zCTriggerOnTouch;
using ZCVobHasEnoughSpace = int(__thiscall*)(void*, zVEC3&); extern ZCVobHasEnoughSpace zCVobHasEnoughSpace;

using ZCVobGetVelocity = zVEC3(__thiscall*)(void*); extern ZCVobGetVelocity zCVobGetVelocity;
using ZCVobBeginMovement = void(__thiscall*)(void* pThis); extern ZCVobBeginMovement  zCVobBeginMovement;

using OCNpcIsMovLock = int(__thiscall*)(oCNpc*); extern OCNpcIsMovLock oCNpcIsMovLock;
using ZCModelCalcModelBBox3DWorld = void(__thiscall*)(zCModel*); extern ZCModelCalcModelBBox3DWorld zCModelCalcModelBBox3DWorld;

using ZCAIPlayerCheckEnoughSpaceMoveDir = int(__thiscall*)(void*, zVEC3 const&, int); extern ZCAIPlayerCheckEnoughSpaceMoveDir zCAIPlayerCheckEnoughSpaceMoveDir;
using OCAIHumanPC_ActionMove = int(__thiscall*)(void* pThis, int); extern OCAIHumanPC_ActionMove oCAIHumanPC_ActionMove;

using  ZCCollObjectCharacterCalcSlideVector = int(__fastcall*)(void* pThis, zVEC3 const& vec1, zVEC3 const& vec2, zVEC3& vec3, zVEC3& vec4, float& floatValue); extern ZCCollObjectCharacterCalcSlideVector zCCollObjectCharacterCalcSlideVector;
using ZMAT4SetTranslation = zMAT4& (__thiscall*)(zMAT4* pThis, zVEC3 const& translation); extern ZMAT4SetTranslation zMAT4SetTranslation;

using ZCVobCalcGroundPoly = void(__thiscall*)(void* pThis); extern ZCVobCalcGroundPoly zCVobCalcGroundPoly;
using  ZCVobUpdatePhysics = void(__thiscall*)(void* pThis); extern ZCVobUpdatePhysics zCVobUpdatePhysics;

//.text:0061E0D0 ; public: virtual void __thiscall zCVob::EndMovement(int)
using ZCVobEndMovement = void(__thiscall*)(void* pThis, int arg0); extern ZCVobEndMovement zCVobEndMovement;

//.text:005501F0; int __cdecl TestStatic_Char_Char(float, class zCCollisionObject *, class zMAT4 const &, class zCCollisionObject *, class zMAT4 const &, class zCCollisionReport * *)
using TestStatic_Char_Char = int(__cdecl*)(float, void* collisionObject1, zMAT4 const& mat1, void* collisionObject2, zMAT4 const& mat2, void* collisionReportPtr); extern TestStatic_Char_Char testStatic_Char_Char;
using ZCCollObjectCharacterTestSoftCollisions = int(__fastcall*)(void* pThis, zVEC3 const& vec1, zVEC3& vec2, zVEC3& vec3, zVEC3& vec4); extern ZCCollObjectCharacterTestSoftCollisions zCCollObjectCharacterTestSoftCollisions;

using ZCCollObjectCharacterTestHardCollisions = int(__fastcall*)(void* pThis, zVEC3 const& vec1, zVEC3 const& vec2, zVEC3& vec3, zTSpatialState* zTSpatialState, zVEC3& vec4); extern ZCCollObjectCharacterTestHardCollisions zCCollObjectCharacterTestHardCollisions;
using ZCCollObjectCharacterDetectCollisionsSelf = void(__fastcall*)(void* pThis, void* collisionObjectArray, void* collisionReports); extern ZCCollObjectCharacterDetectCollisionsSelf zCCollObjectCharacterDetectCollisionsSelf;

using ZCAIPlayerCheckPhysics = void(__thiscall*)(void* pThis); extern ZCAIPlayerCheckPhysics zCAIPlayerCheckPhysics;
using ZCCollObjectCharacterFindFloorWaterCeiling = void(__fastcall*)(void* pThis, zVEC3 const& vec1, void* zTSpatialState); extern ZCCollObjectCharacterFindFloorWaterCeiling zCCollObjectCharacterFindFloorWaterCeiling;

//.text:00706B60 ; protected: virtual void __thiscall oCItemContainer::DrawCategory(void)
using OCItemContainerDrawCategory = void(__thiscall*)(void* pThis); extern OCItemContainerDrawCategory oCItemContainerDrawCategory;

//.text:007092C0 ; public: virtual class oCItem * __thiscall oCItemContainer::GetSelectedItem(void)
using OCItemContainerGetSelectedItem = oCItem * (__thiscall*)(oCItemContainer * pThis); extern OCItemContainerGetSelectedItem oCItemContainerGetSelectedItem;

//.text:00707BA4 ; naked function
using OCItemContainerDrawGetItem = void (*)(); extern OCItemContainerDrawGetItem oCItemContainerDrawGetItem = (OCItemContainerDrawGetItem)0x00707BA4;

//.text:00709740 ; void __thiscall oCItemContainer::NextItem(oCItemContainer *__hidden this)
using OCItemContainerNextItem = void(__thiscall*)(oCItemContainer * pThis); extern OCItemContainerNextItem oCItemContainerNextItem = (OCItemContainerNextItem)0x00709740;

//.text:007076B0 ; void __thiscall oCItemContainer::Draw(oCItemContainer *this)
using OCItemContainerDraw = void(__thiscall*)(oCItemContainer * pThis); extern OCItemContainerDraw oCItemContainerDraw = (OCItemContainerDraw)0x007076B0;

//.text:00705B80 ; int __cdecl oCItemCompareFunction(int,int)
using OCItemCompareFunction = int(__cdecl*)(oCItem * a, oCItem * b); extern OCItemCompareFunction oCItemCompareFunction = (OCItemCompareFunction)0x00705B80;






 //.text:00490EA0 public: class zVEC3 & __thiscall zVEC3::Normalize(void) proc near
 using ZVEC3Normalize = int(__thiscall*)(zVEC3*); extern ZVEC3Normalize zVEC3Normalize;

 //.text:006824D0 public: virtual void __thiscall oCNpc::ResetPos(class zVEC3 &) proc near
 using OCNpcResetPos = void(__thiscall*)(oCNpc*, zVEC3&); extern OCNpcResetPos oCNpcResetPos;

 //0061B720
 using RotateLocalY = void(__thiscall*)(void*, float); extern RotateLocalY rotateLocalY;


 ZVEC3Normalize zVEC3Normalize = (ZVEC3Normalize)0x00490EA0;
 OCNpcResetPos oCNpcResetPos = (OCNpcResetPos)0x006824D0;
 RotateLocalY rotateLocalY = (RotateLocalY)0x0061B720;