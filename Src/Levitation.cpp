#include <Levitation.h>
#include <HookManager.h>
#include <api/g2/zcworld.h>
#include <api/g2/zcinput.h>
#include <api/g2/zcmodel.h>
#include <api/g2/ocgame.h>
#include <LevitationBean.h>
#include <Logger.h>
#include <Util.h>
#include <queue>
#include <DynItemInst.h>

using namespace LevitationUtil;

typedef void(__thiscall* ZCVobDoFrameActivity)(void*); ZCVobDoFrameActivity zCVobDoFrameActivity;
typedef void(__thiscall* ZCVobSetPhysicsEnabled)(void*, int); ZCVobSetPhysicsEnabled zCVobSetPhysicsEnabled;
typedef void(__cdecl* ZCVobCheckAutoUnlink)(zCVob*); ZCVobCheckAutoUnlink zCVobCheckAutoUnlink;
typedef void(__thiscall* OCGamePause)(void*, int); OCGamePause oCGamePause;
typedef void(__thiscall* OCGameUnpause)(void*); OCGameUnpause oCGameUnpause;
typedef void(__thiscall* ZCVobSetCollDet)(void*, int); ZCVobSetCollDet zCVobSetCollDet;
typedef int(__thiscall* ZCAIPlayerCheckFloorSliding)(void*); ZCAIPlayerCheckFloorSliding zCAIPlayerCheckFloorSliding;
typedef void(__thiscall* DoSurfaceAlignment)(void*); DoSurfaceAlignment doSurfaceAlignment;
typedef void(__thiscall* ZCVobCheckAndResolveCollisions)(void*); ZCVobCheckAndResolveCollisions zCVobCheckAndResolveCollisions;
typedef void(__thiscall* ZCTriggerOnTouch)(void*, zCVob*); ZCTriggerOnTouch zCTriggerOnTouch;
typedef int(__thiscall* ZCVobHasEnoughSpace)(void*, zVEC3&); ZCVobHasEnoughSpace zCVobHasEnoughSpace;
typedef zVEC3(__thiscall* ZCVobGetVelocity)(void*); ZCVobGetVelocity zCVobGetVelocity;

//.text:007380E0 public: int __thiscall oCNpc::IsMovLock(void) proc near
typedef int(__thiscall* OCNpcIsMovLock)(oCNpc*);
OCNpcIsMovLock oCNpcIsMovLock = (OCNpcIsMovLock)0x007380E0;

//.text:00579170 public: void __thiscall zCModel::CalcModelBBox3DWorld(void) proc near
typedef void(__thiscall* ZCModelCalcModelBBox3DWorld)(zCModel*);
ZCModelCalcModelBBox3DWorld zCModelCalcModelBBox3DWorld = (ZCModelCalcModelBBox3DWorld)0x00579170;

//.text:00511320 public: int __thiscall zCAIPlayer::CheckEnoughSpaceMoveDir(class zVEC3 const &, int) proc near
typedef int(__thiscall* ZCAIPlayerCheckEnoughSpaceMoveDir)(void*, zVEC3 const &, int); ZCAIPlayerCheckEnoughSpaceMoveDir zCAIPlayerCheckEnoughSpaceMoveDir;

namespace LevitationUtil
{
	ZVEC3Normalize zVEC3Normalize = (ZVEC3Normalize)0x00490EA0;
	OCNpcResetPos oCNpcResetPos = (OCNpcResetPos)0x006824D0;
	//.text:0061E0D0 public: virtual void __thiscall zCVob::EndMovement(int) proc near
	ZCVobEndMovement zCVobEndMovement = (ZCVobEndMovement)0x0061E0D0;
}

int COLLISION_HEAD_SIZE_Y = 25;
bool Levitation::adjustHeroPosition = false;
bool Levitation::pausedGame = false;
bool Levitation::noCollision = false;
int Levitation::frameTime = 0;
int Levitation::diffFrameTime = 0;
float Levitation::yPos = 0;

LevitationBean* heroLevitationBean;
bool heroLevitationBeanCalled;
bool fakeUpKey;
int frameTimePast;



Levitation::Levitation()
	:Module()
{
	moduleDesc = "Levitation";
}

Levitation::~Levitation()
{
}

void Levitation::hookModule()
{
	zCVobDoFrameActivity = reinterpret_cast<ZCVobDoFrameActivity>(ZCVOB_DO_FRAME_ACTIVITY_ADDRESS);
	zCVobSetPhysicsEnabled = reinterpret_cast<ZCVobSetPhysicsEnabled>(ZCVOB_SET_PHYSICS_ENABLED_ADDRESS);
	oCGamePause = reinterpret_cast<OCGamePause>(OCGAME_PAUSE_ADDRESS);
	oCGameUnpause = reinterpret_cast<OCGameUnpause>(OCGAME_UNPAUSE_ADDRESS);

	zCVobCheckAutoUnlink = reinterpret_cast<void(__cdecl*)(zCVob*)>(ZCVOB_CHECK_AUTO_UNLINK_ADDRESS);
	zCVobSetCollDet = reinterpret_cast<void(__thiscall*)(void*, int)>(ZCVOB_SET_COLL_DET_ADDRESS);
	zCAIPlayerCheckFloorSliding = reinterpret_cast<int(__thiscall*)(void*)>((ZCAIPLAYER_CHECK_FLOOR_SLIDING_ADDRESS));

	doSurfaceAlignment = reinterpret_cast<DoSurfaceAlignment>((DO_SURFACE_ALIGNMENT_ADDRESS));

	zCTriggerOnTouch = reinterpret_cast<ZCTriggerOnTouch>(ZCTRIGGER_ON_TOUCH_ADDRESS);

	zCVobHasEnoughSpace = reinterpret_cast<ZCVobHasEnoughSpace>(ZCVOB_HAS_ENOUGH_SPACE_ADDRESS);

	zCVobCheckAndResolveCollisions = reinterpret_cast<ZCVobCheckAndResolveCollisions>((ZCVOB_CHECK_AND_RESOLVE_COLLISION_ADDRESS));
	zCVobGetVelocity = reinterpret_cast<zVEC3(__thiscall*)(void*)>(ZCVOB_GET_VELOCITY_ADDRESS);

	zCAIPlayerCheckEnoughSpaceMoveDir = (ZCAIPlayerCheckEnoughSpaceMoveDir)0x00511320;

	HookManager* hookManager = HookManager::getHookManager();
	hookManager->addFunctionHook((LPVOID*)&zCVobDoFrameActivity, zCVobDoFrameActivityHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCVobSetPhysicsEnabled, zCVobSetPhysicsEnabledHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCGamePause, oCGamePauseHook, moduleDesc);

	hookManager->addFunctionHook((LPVOID*)&oCGameUnpause, oCGameUnpauseHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&doSurfaceAlignment, DoSurfaceAlignmentHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCVobCheckAndResolveCollisions, zCVobCheckAndResolveCollisionsHook, moduleDesc);

	hookManager->addFunctionHook((LPVOID*)&zCTriggerOnTouch, zCTriggerOnTouchHook, moduleDesc);
	hookManager->addFunctionHook((LPVOID*)&zCVobHasEnoughSpace, zCVobHasEnoughSpaceHook, moduleDesc);

	hookManager->addFunctionHook((LPVOID*)&zCAIPlayerCheckEnoughSpaceMoveDir, zCAIPlayerCheckEnoughSpaceMoveDirHook, moduleDesc);
}

void Levitation::unHookModule()
{
}


void Levitation::zCVobDoFrameActivityHook(void* pThis)
{
	void(__thiscall*BeginMovement)(void*);
	BeginMovement = reinterpret_cast<void(__thiscall*)(void*)>(0x0061DA80);//0061DA80

	void(__thiscall*EndMovement)(void*, int);
	EndMovement = reinterpret_cast<void(__thiscall*)(void*, int)>(0x0061E0D0);//0061E0D0

	void(__thiscall*RotateLocalY)(void*, float);
	RotateLocalY = reinterpret_cast<void(__thiscall*)(void*, float)>(0x0061B720);//0061B720

	oCNpc* hero = oCNpc::GetHero();
	if (!heroLevitationBeanCalled) {
		heroLevitationBean = new LevitationBean(hero);
		heroLevitationBeanCalled = true;
	}
	bool adjust = (hero == pThis) && Levitation::adjustHeroPosition;
	float oldYPosition = 0;
	zVEC3 oldLook;
	if (adjust) {
		oldYPosition = hero->GetPosition().y;
		zMAT4* mat = &hero->trafoObjToWorld;
		oldLook = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
		heroLevitationBean->oldXPos = mat->m[0][3];
		heroLevitationBean->oldYPos = mat->m[1][3];
		heroLevitationBean->oldZPos = mat->m[2][3];
	}

	zCVobDoFrameActivity(pThis);

	if (pThis == hero)
	{
		frameTimePast = calcPastFrameTime();
	}
	else
	{
		//zCVobDoFrameActivity(pThis);
	}
	if (adjust) {
		Levitation::noCollision = false;
		//BeginMovement(hero);
		Levitation::zCVobSetCollDetHook(hero, 1);
		hero->ResetXZRotationsLocal();
		zCInput* input = zCInput::GetInput();
		int arrowLeft = input->KeyPressed(203);
		int arrowRight = input->KeyPressed(205);
		int arrowUp = input->KeyPressed(200);

		if (!arrowLeft && !arrowRight)
		{
			//adjust Rotation;
			hero->ResetXZRotationsLocal();
			zMAT4* mat = &hero->trafoObjToWorld;
			mat->m[0][2] = oldLook.x;
			mat->m[1][2] = oldLook.y;
			mat->m[2][2] = oldLook.z;
		}

		if (arrowLeft)
		{
			float speed = -90.0;
			float angle = speed * float(frameTimePast) / 1000;
			RotateLocalY(hero, angle);
		}

		if (arrowRight)
		{
			float speed = 90.0;
			float angle = speed * float(frameTimePast) / 1000;
			RotateLocalY(hero, angle);
		}

		//hero->ResetXZRotationsWorld();
		//Levitation::zCVobCheckAndResolveCollisionsHook(hero);
		zCVobSetPhysicsEnabled(hero, true);
		Levitation::noCollision = true;

		zVEC3 pos = hero->GetPosition();
		if (pos.y < oldYPosition) {
			hero->SetPositionWorld(zVEC3(pos.x, oldYPosition, pos.z));
		}

		//hero->SetPositionWorld(zVEC3(pos.x, pos.y, pos.z));
		/*if (heroLevitationBean->getMoveDown())
		{
		float speed = 4*50.0f; //100cm per second
		float moveDownDistance = speed * float(frameTimePast)/ 1000;
		pos = hero->GetPosition();
		//pos.y -= moveDownDistance;
		//hero->SetPositionWorld(pos);
		} */
		float hoverDistance = heroLevitationBean->getHoverDistance();
		if (hoverDistance < heroLevitationBean->HOVER_DISTANCE)
		{
			//fakeUpKey = true;
			float speed = 10.0f; //100cm per second
			float moveUpDistance = speed * float(frameTimePast) / 1000;
			pos = hero->GetPosition();
			pos.y += moveUpDistance;
			//hero->SetPositionWorld(pos);
			heroLevitationBean->setHoverDistance(heroLevitationBean->getDistanceToGround(pos));
			//heroLevitationBean->increaseHoverDistance(moveUpDistance);
		}
		levitate();
		Levitation::noCollision = false;

		int(__thiscall*IsColliding)(void*);
		IsColliding = reinterpret_cast<int(__thiscall*)(void*)>(0x0061F4F0);//0061F4F0

		void(__thiscall*ResetCollisionObjectState)(void*);
		ResetCollisionObjectState = reinterpret_cast<void(__thiscall*)(void*)>(0x0061E690);//0061E690
		
		void(__thiscall*CreateTrafoLocal)(void*);
		CreateTrafoLocal = reinterpret_cast<void(__thiscall*)(void*)>(0x0061D100);//0061D100

		void(__fastcall*CalcDestinationBBox3DWorld)(void*, zTBBox3D &);
		CalcDestinationBBox3DWorld = reinterpret_cast<void(__fastcall*)(void*, zTBBox3D &)>(0x0061DEE0);//0061DEE0

		int(__thiscall*HasNpcEnoughSpace)(void*, zVEC3 &);
		HasNpcEnoughSpace = reinterpret_cast<int(__thiscall*)(void*, zVEC3 &)>(0x0077C3C0);//0077C3C0
																						   //Levitation::zCVobCheckAndResolveCollisionsHook(hero);
		Test(hero);
		//DynItemInst::yPos = hero->GetPositionWorld().y;
		fakeUpKey = false;
		//EndMovement(hero, 1);
		//BeginMovement(hero);
		float speed = 100.0f; //100cm per second
		float distance = speed * float(frameTimePast) / 1000;
		zVEC3 newPos = hero->GetPosition();
		newPos.y -= distance;
		//hero->GetModel()->CalcModelBBox3DWorld();
		//hero->CreateCollisionObject();
		//ResetCollisionObjectState(hero);
		//CreateTrafoLocal(hero);
		//Levitation::zCVobCheckAndResolveCollisionsHook(hero);
		//CalcDestinationBBox3DWorld(hero, hero->GetBBox3DLocal());

		void(__fastcall*DetectCollisions)(void*, void*, zCArray<void*> &, zCArray<void*> &);
		DetectCollisions = reinterpret_cast<void(__fastcall*)(void*, void*, zCArray<void*> &, zCArray<void*> &)>(0x0054CE50);//0054CE50

		void(__thiscall*zCCollisionDetector)(void*);
		zCCollisionDetector = reinterpret_cast<void(__thiscall*)(void*)>(0x0054C5E0);//0054C5E0


		//zCArray<void*> collObjects;
		//zCArray<void*> collReports;
		//zCCollisionDetector(detector);
		//DetectCollisions(0, hero->m_poCollisionObject, collObjects, collReports);
		//if (collObjects.GetSize() || collReports.GetSize())
		//{
		//	std::cout << "Collision detected!" << std::endl;	
		//}
		//EndMovement(hero, 1);
		//std::cout << hero->DetectCollision(&hero->trafoObjToWorld) << std::endl;
	}
}

void Levitation::zCVobSetPhysicsEnabledHook(void* pThis, int second)
{
	oCNpc* hero = oCNpc::GetHero();
	if (hero != NULL && (pThis == hero) && Levitation::adjustHeroPosition) {
		zCInput* input = zCInput::GetInput();
		//if (input->KeyPressed(0x1A)) return; //zCVobSetPhysicsEnabled(pThis, true);
		return;
	}
	zCVobSetPhysicsEnabled(pThis, second);
}

void Levitation::oCGamePauseHook(void* pThis, int second)
{
	logStream << "Paused Game!" << std::endl;
	util::debug(&logStream);
	Levitation::pausedGame = true;
	oCGamePause(pThis, second);
}

void Levitation::oCGameUnpauseHook(void* pThis)
{
	logStream << "Unpaused Game!" << std::endl;
	util::debug(&logStream);
	Levitation::pausedGame = false;
	oCGameUnpause(pThis);
}

void Levitation::zCVobSetCollDetHook(void* pThis, int second)
{
	oCNpc* hero = oCNpc::GetHero();
	if (hero != nullptr && (pThis == hero) && adjustHeroPosition) {
		return zCVobSetCollDet(pThis, 0);
	}
	zCVobSetCollDet(pThis, second);
}

int Levitation::zCAIPlayerCheckFloorSlidingHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	if (hero != NULL && (pThis == hero) && adjustHeroPosition) {
		return 1;
	}
	return zCAIPlayerCheckFloorSliding(pThis);
}

void Levitation::DoSurfaceAlignmentHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	bool adjust = (hero == pThis) && Levitation::adjustHeroPosition;
	if (adjust) {
		return;
	}
	return doSurfaceAlignment(pThis);
}

void Levitation::zCVobCheckAndResolveCollisionsHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	bool adjust = (hero != NULL) && (hero == pThis) && Levitation::adjustHeroPosition;
	if (adjust) {
		//if (Levitation::noCollision && !DynItemInst::loadGame && !HookManager::changeLevel) return;
		return;
	}
	zCVobCheckAndResolveCollisions(pThis);
}

void Levitation::zCTriggerOnTouchHook(void* pThis, zCVob* vob)
{
	if (vob == oCNpc::GetHero()) {
		logStream << "zCTriggerOnTouch() called on hero!!" << std::endl;
		util::debug(&logStream);
		zCTriggerOnTouch(pThis, vob);
	}
}

int Levitation::zCVobHasEnoughSpaceHook(void* pThis, zVEC3& second)
{
	int result = zCVobHasEnoughSpace(pThis, second);
	if (pThis == oCNpc::GetHero()) {
		logStream << "called for hero!: " << result << std::endl;
		util::debug(&logStream);
	}
	return result;
}

zVEC3 Levitation::zCVobGetVelocityHook(void* pThis)
{
	return zCVobGetVelocity(pThis);
}

int Levitation::zCVobIsColliding(void* pThis)
{
	XCALL(ZCVOB_IS_COLLIDING);
}

int Levitation::zCBspBaseCollectPolysInBBox3D(void*, zTBBox3D const&, zCPolygon**&, int&)
{
	XCALL(ZCBSPBASE_COLLECT_POLYS_IN_BBOX3D);
}

int Levitation::zCAIPlayerCheckEnoughSpaceMoveDirHook(void* pThis, zVEC3 const& vec, int second)
{
	int result = zCAIPlayerCheckEnoughSpaceMoveDir(pThis, vec, second);
	oCNpc* hero = oCNpc::GetHero();
	if (result && hero != nullptr && (pThis == hero) && adjustHeroPosition) {
		logStream << "zCAIPlayerCheckEnoughSpaceMoveDirHook: called!"  << std::endl;
		util::logInfo(&logStream);
		return 0;
	}

	return result;
}

void levitate() {
	if (!Levitation::adjustHeroPosition) return;
	if (Levitation::pausedGame) return;

	std::stringstream logStream;

	oCNpc* hero = oCNpc::GetHero();
	if (oCNpcIsMovLock(hero)) return;

	//TODO: mop interaction!!!
	/*oCMobInter* mobInter = hero->GetInteractMob();
	if (mobInter != NULL) return;*/
	//zCVobSetPhysicsEnabled(hero, true);
	Levitation::noCollision = true;
	float speed = 100.0f; //100cm per second
	float distance = speed * float(frameTimePast) / 1000;
	zVEC3 pos = hero->GetPositionWorld();
	zCInput* input = zCInput::GetInput();

	zCModelCalcModelBBox3DWorld(hero->GetModel());// ->CalcModelBBox3DWorld();
	zTBBox3D bBox = LevitationBean::zCModelGetBBox3D(hero->GetModel());// ->GetBBox3D();
	zMAT4* mat = &(hero->trafoObjToWorld);
	float yUpExt = bBox.bbox3D_maxs.y;
	float yDownExt = bBox.bbox3D_mins.y;


	//DynItemInst::zCVobSetCollDetHook(hero, 0);
	// Pressed � (levitation up)
	if (input->KeyPressed(0x1A)) {
		float collisionYUpDist = getCollideYDir(hero->GetVobPosition(), abs(yUpExt), true).distance;
		//collisionYPos = -1;
		if (collisionYUpDist > COLLISION_HEAD_SIZE_Y) {
			hero->SetPositionWorld(zVEC3(pos.x, pos.y + distance, pos.z));
			zVEC3 pos = hero->GetPosition();
			heroLevitationBean->setHoverDistance(heroLevitationBean->getDistanceToGround(pos));
		}
		//npc->SetPositionWorld(zVEC3(pos.x, pos.y + distance, pos.z));

		// Pressed � (levitation down)
	}
	else if (input->KeyPressed(0x28)) {
		float collisionYDownDist = getCollideYDir(hero->GetVobPosition(), abs(5 * yDownExt / 6), false).distance;
		if (collisionYDownDist > abs(yDownExt / 6)) {
			//writeToConsole("DownDist: ", collisionYDownDist);
			hero->SetPositionWorld(zVEC3(pos.x, pos.y - distance, pos.z));
			zVEC3 pos = hero->GetPosition();
			heroLevitationBean->setHoverDistance(heroLevitationBean->getDistanceToGround(pos));
			//hero->Move(0, -distance, 0);
		}
	}

	// Pressed Arrow_Up (levitation forward)
	if (input->KeyPressed(0xC8)) {
		zVEC3 look = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
		float collisionForward1 = getCollideForwardDir(hero, yUpExt / 2, false).distance;
		float collisionForward2 = getCollideForwardDir(hero, yUpExt / 4, false).distance;
		float collisionForward = min(collisionForward1, collisionForward2);

		//float collisionYUpDist = getCollideYDir(hero->GetVobPosition() + look*300, 0, true).distance;
		//collisionYPos = -1;
		logStream << "dist: " << collisionForward << std::endl;
		util::debug(&logStream);

		//logStream << "levitate: collisionYUpDist= " << collisionYUpDist << std::endl;
		//util::logInfo(&logStream);

		if (collisionForward > 15)// && (collisionYUpDist > yUpExt))
		{
			float factor = min(collisionForward, 100);
			zVEC3 lookBackup = look;
			look.x *= factor;
			look.y *= factor;
			look.z *= factor;
			//hero->Move(5*look.x, 5*look.y, 5*look.z);
			zVEC3 testPos = hero->GetPositionWorld();
			look.x += testPos.x;
			look.y += testPos.y;
			look.z += testPos.z;
			//testPos.x += look.x*1;
			//testPos.y += look.y*1;
			//testPos.z += look.z*1;
			//zMAT4 testMat = *mat;
			//testMat.m[0][3] = testPos.x;
			//testMat.m[1][3] = testPos.y;
			//testMat.m[2][3] = testPos.z;
			//zCVobSetPhysicsEnabled(hero, false);
			//writeToConsole("HasEnoughSpace: ", zCVobHasEnoughSpace(hero, testPos));
			//hero->SetPosition(0,-100000,0);
			/*
			int coll = hero->DetectCollision(&testMat);
			writeToConsole("HasEnoughSpace: ", coll);
			if (coll) {
			zCVobHasEnoughSpace(hero, testPos);
			zCVobCheckAndResolveCollisions(hero);
			}
			*/

			//hero->Move(lookBackup.x*5, lookBackup.y*5, lookBackup.z*5);
			zVEC3 posHero = hero->GetPositionWorld();
			hero->SetPositionWorld(zVEC3(posHero.x + lookBackup.x * 5, posHero.y + lookBackup.y * 5, posHero.z + lookBackup.z * 5));
			zCWorld* world = oCGame::GetGame()->GetWorld();
			int flags = (1 << 0) | (1 << 5) | (1 << 6);
			LevitationBean::zCWorldTraceRayNearestHit(world, testPos, look, nullptr, flags);
			zCPolygon* poly = world->foundPoly;
			if (poly != NULL) {
				zCMaterial* material = *reinterpret_cast<zCMaterial**>(reinterpret_cast<char *>(poly) +
					zCPolygonOffsets::Offset_Material);
				if ((material != NULL) && zCMaterial::IsPortalMaterial(material)) {
					//writeToConsole("Got Portal material!");
					//zCVobHasEnoughSpace(hero, posHero);
				}

			}
			else {
				//Nothing was found. Savely outwit collision
				// Notice: Dirty hack; no collision will be recognised!
				//DynItemInst::noCollision = true;
				//zCVobSetPhysicsEnabled(hero, false);
				//zCVobSetCollDet(hero, false);
			}
		}

		// Pressed Arrow_Down (levitation backward)
	}
	else if (input->KeyPressed(0xD0)) {
		//Motion motion = getCollideForwardDir(hero, 0, true);
	}
	//hero->CreateCollisionObject();
};

Motion getCollideYDir(zVEC3 pos, float middlePointDist, bool up) {
	zVEC3 dir;
	if (up) {
		pos.y += middlePointDist;
		dir = pos;
		dir.y += 1000000000;
	}
	else {
		//zTBBox3D bBox = LevitationBean::zCModelGetBBox3D(npc->GetModel());//->GetBBox3D();
		//zVEC3 maxs = bBox.bbox3D_maxs;
		//pos.y -= middlePointDist;
		//if (maxs.x < 0 || maxs.z < 0) {
			//writeToConsole("Attentation: maxs member have unexpected values!");
		//}
		//zMAT4* mat = &npc->trafoObjToWorld;
		//zVEC3 look = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
		//if (look.x == 0 || look.z == 0) {
			//writeToConsole("Attentation2: look member have unexpected values!");
		//}
		pos.y += middlePointDist;
		//pos.x += maxs.x*look.x;
		//pos.z += maxs.z*look.z;
		dir = pos;
		//dir.y -= 30000;
		dir.y -= 1000000000;
	}
	zCWorld* world = oCGame::GetGame()->GetWorld();
	int flags = (1 << 0) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 14);
	LevitationBean::zCWorldTraceRayNearestHit(world, pos, dir, nullptr, flags);
	zVEC3* intersection = &(world->foundIntersection);

	Motion motion;
	motion.normal = world->foundPolyNormal;
	motion.distance = abs(intersection->y - pos.y);
	motion.objectPos = pos;
	motion.intersect = *intersection;
	//writeToConsole("hero.y:     ", npc->GetPositionWorld().y);
	//writeToConsole("pos.y:      ", pos.y);
	//writeToConsole("interset.y: ", intersection->y);
	return motion;
};

Motion getCollideForwardDir(oCNpc* npc, float middlepointDistance, bool backward) {
	zMAT4* mat = &(npc->trafoObjToWorld);
	zVEC3 dir = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
	// position inside the BBox
	float headSize = 30.0f;
	zVEC3 pos = zVEC3(mat->m[0][3], mat->m[1][3] + middlepointDistance + headSize, mat->m[2][3]);
	pos.x += dir.x * 10;
	pos.y += dir.y * 10;
	pos.z += dir.z * 10;
	if (backward) {
		dir = zVEC3(-dir.x, -dir.y, -dir.z);
	}

	float distanceFactor = 1000000000;
	// dir has length 1 
	// we want a vector that is 10000 units away from pos in direction dir
	zVEC3 posOutside = zVEC3(pos.x + dir.x*distanceFactor, pos.y + dir.y*distanceFactor, pos.z + dir.z*distanceFactor);

	zCWorld* world = oCGame::GetGame()->GetWorld();
	int flags = (1 << 0) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 14);
	LevitationBean::zCWorldTraceRayNearestHit(world, pos, posOutside, nullptr, flags);
	zVEC3* intersection = &(world->foundIntersection);

	Motion motion;
	motion.normal = world->foundPolyNormal;
	motion.distance = abs((intersection->x - pos.x) + (intersection->y - pos.y) + (intersection->z - pos.z));
	motion.objectPos = pos;
	motion.intersect = *intersection;
	return motion;
};


int calcPastFrameTime() {
	int time = sysGetTime();
	int diff = time - Levitation::frameTime;
	Levitation::frameTime = time;
	Levitation::diffFrameTime = diff;
	return diff;
};

zVEC3 getCollidingPolyNormal(oCNpc* hero, const zMAT4& mat)
{
	bool intersected = false;
	zVEC3 pos(mat.m[0][3], mat.m[1][3], mat.m[2][3]);
	zCPolygon** polys;
	void* pointer = (void*)*(int*)((BYTE*)hero + 0x0b8);
	pointer = (void*)*(int*)((BYTE*)pointer + 0x1b4);
	int third = 0;

	zCModel* model = hero->GetModel();
	zCModelCalcModelBBox3DWorld(model);
	zTBBox3D box = LevitationBean::zCModelGetBBox3D(model);

	//make rec a little bit bigger than box
	zTBBox3D rec;
	rec.bbox3D_maxs = zVEC3(100, 100, 100);//box.bbox3D_maxs;//zVEC3(100,100,100);
										   //logStream << "Test:  box.bbox3D_maxs: " << box.bbox3D_maxs << std::endl;
										   //util::logInfo(&logStream);
	rec.bbox3D_mins = zVEC3(-100, -100, -100);// box.bbox3D_mins;//zVEC3(-100, -100, -100);
											  //logStream << "Test:  box.bbox3D_mins: " << box.bbox3D_mins << std::endl;
											  //util::logInfo(&logStream);
	rec.bbox3D_maxs += pos;
	rec.bbox3D_mins += pos;
	box.bbox3D_maxs += pos;
	box.bbox3D_mins += pos;

	Levitation::zCBspBaseCollectPolysInBBox3D(pointer, rec, polys, third);
	if (polys != nullptr) {
		//writeToConsole("Collision detected: ", third);
		//logStream << "Test: Collision detected: " << third << std::endl;
		//util::logInfo(&logStream);
		if (third)
		{
			//create plane from intersecting poly
			Plane plane;
			zVEC3 look = zVEC3(mat.m[0][2], mat.m[1][2], mat.m[2][2]);
			zVEC3Normalize(&look);
			zVEC3 oldPos = zVEC3(heroLevitationBean->oldXPos, heroLevitationBean->oldYPos, heroLevitationBean->oldZPos);
			zVEC3 dir = pos - oldPos;
			zVEC3Normalize(&dir);

			Plane::initPlaneByVectors(plane, pos, look);
			plane.convertToHNF();
			Line motionLine(oldPos, look);

			for (int i = 0; i < third; ++i)
			{
				zCPolygon* poly = polys[i];

				if (poly->CheckBBoxPolyIntersection(rec))
				{
					if (poly->CheckBBoxPolyIntersection(box))
					{
						zVEC3* polyNormal = poly->getPolyNormal();

						//we have found what we wanted -> a collision!
						return zVEC3(polyNormal->x, polyNormal->y, polyNormal->z);
						
					}
				}
			}
		}
	}

	//nothing found - return null vector
	return zVEC3(0,0,0);
}


bool checkCollision(oCNpc* hero, const zMAT4& mat)
{
	bool intersected = false;
	zVEC3 pos (mat.m[0][3], mat.m[1][3], mat.m[2][3]);
	zCPolygon** polys;
	void* pointer = (void*)*(int*)((BYTE*)hero + 0x0b8);
	pointer = (void*)*(int*)((BYTE*)pointer + 0x1b4);
	int third = 0;

	zCModel* model = hero->GetModel();
	zCModelCalcModelBBox3DWorld(model);
	zTBBox3D box = LevitationBean::zCModelGetBBox3D(model);

	//make rec a little bit bigger than box
	zTBBox3D rec;
	rec.bbox3D_maxs = zVEC3(100, 100, 100);//box.bbox3D_maxs;//zVEC3(100,100,100);
										   //logStream << "Test:  box.bbox3D_maxs: " << box.bbox3D_maxs << std::endl;
										   //util::logInfo(&logStream);
	rec.bbox3D_mins = zVEC3(-100, -100, -100);// box.bbox3D_mins;//zVEC3(-100, -100, -100);
											  //logStream << "Test:  box.bbox3D_mins: " << box.bbox3D_mins << std::endl;
											  //util::logInfo(&logStream);
	rec.bbox3D_maxs += pos;
	rec.bbox3D_mins += pos;
	box.bbox3D_maxs += pos;
	box.bbox3D_mins += pos;

	Levitation::zCBspBaseCollectPolysInBBox3D(pointer, rec, polys, third);
	if (polys != nullptr) {
		//writeToConsole("Collision detected: ", third);
		//logStream << "Test: Collision detected: " << third << std::endl;
		//util::logInfo(&logStream);
		if (third)
		{
			//create plane from intersecting poly
			Plane plane;
			zVEC3 look = zVEC3(mat.m[0][2], mat.m[1][2], mat.m[2][2]);
			zVEC3Normalize(&look);
			zVEC3 oldPos = zVEC3(heroLevitationBean->oldXPos, heroLevitationBean->oldYPos, heroLevitationBean->oldZPos);
			zVEC3 dir = pos - oldPos;
			zVEC3Normalize(&dir);

			Plane::initPlaneByVectors(plane, pos, look);
			plane.convertToHNF();
			Line motionLine(oldPos, look);

			float globalLambda = 0;

			for (int i = 0; i < third; ++i)
			{
				zCPolygon* poly = polys[i];

				//if (poly->CheckBBoxPolyIntersection(rec))
				//{
					if (poly->CheckBBoxPolyIntersection(box))
					{
						if (!intersected) {
							intersected = true;

							//we have found what we wanted -> a collision!
							break;
						}
					}
					/*for (int j = 0; j < poly->GetNumPolyVertices(); ++j) {
					zCVertex* vertex = poly->getVertices()[j];
					zVEC3 vertPos = zVEC3(vertex->Position[0], vertex->Position[1], vertex->Position[2]);

					Plane vertPlane;
					Plane::initPlaneByVectors(vertPlane, vertPos, look);
					vertPlane.convertToHNF();
					// should always be true(?)
					if (vertPlane.isLineIntersecting(motionLine))
					{
					float lambda = vertPlane.getLambdaIntersectionValueForLine(motionLine);
					zVEC3 intersectPoint = motionLine.getPointByLambda(lambda);
					float distanceToIntersect = getDistanceOfVectors(oldPos, intersectPoint);
					float distanceToPos = getDistanceOfVectors(oldPos, pos);
					if (distanceToIntersect < distanceToPos || Float::areEqual(distanceToIntersect, distanceToPos))
					{
					if (globalLambda > lambda) {
					globalLambda = lambda;
					}
					}
					}
					}*/
				//}
			}

			// update position of the npc
			/*if (globalLambda != 0) {
			logStream << "Test: globalLambda = " << globalLambda << std::endl;
			util::logInfo(&logStream);
			pos = motionLine.getPointByLambda(globalLambda);
			hero->SetPositionWorld(pos);
			} else */


		}
	}

	return intersected;
}


void Test(oCNpc* hero) {

	std::stringstream logStream;
	logStream << "Test: called!" << std::endl;
	util::logInfo(&logStream);
	//Motion collideDown = getCollideYDir(hero, 98, false);
	zTBBox3D bBox = LevitationBean::zCModelGetBBox3D(hero->GetModel());// ->GetBBox3D();
	zVEC3 pos = hero->GetPosition();
	zMAT4* mat = &(hero->trafoObjToWorld);
	float yUpExt = bBox.bbox3D_maxs.y;
	//writeToConsole("diff: ", diff);
	//Motion collideUp = getCollideYDir(hero, abs(2*yDownExt/3), false);
	Motion collideDown = getCollideYDir(hero->GetVobPosition(), 0, false);
	zVEC3 iSect = collideDown.intersect;
	zVEC3 objPos = collideDown.objectPos;
	float yIntersect = objPos.y - iSect.y; // always > 0
	zCInput* input = zCInput::GetInput();
	bool upKeyPressed = input->KeyPressed(0x1A); // � = 0x1A
	bool downKeyPressed = input->KeyPressed(0x28); // � = 0x1A
												   //writeToConsole("yIntersect: ", yIntersect);
												   //if (yIntersect < abs(yDownExt/2) && !upKeyPressed) {
	heroLevitationBean->setMoveDown(false);
	//writeToConsole("before: ", heroLevitationBean->getHoverDistance());
	float distanceToGround = heroLevitationBean->getDistanceToGround(pos);
	heroLevitationBean->update(distanceToGround);
	//writeToConsole("after: ", heroLevitationBean->getHoverDistance());
	if (yIntersect < heroLevitationBean->HOVER_DISTANCE && !upKeyPressed) {
		//TODO : check status of previously initialized LevitationBean!
		float hoverDistance = heroLevitationBean->getHoverDistance();
		if (distanceToGround < hoverDistance)
		{
			//Collision has occured!
			//writeToConsole("Collision detected!!!");
			//pos.y += collideDown.distance;
			//hero->SetPositionWorld(zVEC3(pos.x, iSect.y + 80, pos.z));
			//hero->SetPositionWorld(zVEC3(pos.x, iSect.y + hoverDistance, pos.z));
			//hero->SetPositionWorld(zVEC3(pos.x, iSect.y + abs(yDownExt) - 5, pos.z));
			//}	
		}
		//hero->SetPosition(pos.x, iSect.y + hoverDistance, pos.z); TODO!!!!!

		//hero->SetPositionWorld(zVEC3(pos.x, iSect.y + 80, pos.z));
	}
	else if (yIntersect < 120 && !upKeyPressed) {
		//heroLevitationBean->setMoveDown(true);
	}

	zCPolygon** polys;
	void* pointer = (void*)*(int*)((BYTE*)hero + 0x0b8);
	pointer = (void*)*(int*)((BYTE*)pointer + 0x1b4);
	int third = 0;

	zCModel* model = hero->GetModel();
	zCModelCalcModelBBox3DWorld(model);
	zTBBox3D box = LevitationBean::zCModelGetBBox3D(model);

	//make rec a little bit bigger than box
	zTBBox3D rec; 
	rec.bbox3D_maxs = box.bbox3D_maxs;//zVEC3(100,100,100);
	box.bbox3D_maxs.x = 20;
	box.bbox3D_maxs.z *= 0.5;
	//logStream << "Test:  box.bbox3D_maxs: " << box.bbox3D_maxs << std::endl;
	//util::logInfo(&logStream);
	rec.bbox3D_mins = box.bbox3D_mins;//zVEC3(-100, -100, -100);
	box.bbox3D_mins.x = 20;
	box.bbox3D_mins.z *= 0.5;
	//logStream << "Test:  box.bbox3D_mins: " << box.bbox3D_mins << std::endl;
	//util::logInfo(&logStream);
	rec.bbox3D_maxs += pos;
	rec.bbox3D_mins += pos;
	box.bbox3D_maxs += pos;
	box.bbox3D_mins += pos;

	Levitation::zCBspBaseCollectPolysInBBox3D(pointer, rec, polys, third);
	if (polys != nullptr) {
		//writeToConsole("Collision detected: ", third);
			//logStream << "Test: Collision detected: " << third << std::endl;
			//util::logInfo(&logStream);
		if (third)
		{
			//create plane from intersecting poly
			Plane plane;
			zVEC3 look = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
			zVEC3Normalize(&look);
			zVEC3 pos = zVEC3(mat->m[0][3], mat->m[1][3], mat->m[2][3]);
			zVEC3 oldPos = zVEC3(heroLevitationBean->oldXPos, heroLevitationBean->oldYPos, heroLevitationBean->oldZPos);
			zVEC3 dir = pos - oldPos;
			zVEC3Normalize(&dir);

			Plane::initPlaneByVectors(plane, pos, look);
			plane.convertToHNF();
			Line motionLine(oldPos, look);
			bool intersected = false;

			float globalLambda = 0;

			for (int i = 0; i < third; ++i)
			{
				zCPolygon* poly = polys[i];

				//if (poly->CheckBBoxPolyIntersection(rec))
				//{
					if (poly->CheckBBoxPolyIntersection(box))
					{
						if (!intersected) {
							intersected = true;

							//we have found what we wanted -> a collision!
							break;
						}
					}
					/*for (int j = 0; j < poly->GetNumPolyVertices(); ++j) {
						zCVertex* vertex = poly->getVertices()[j];
						zVEC3 vertPos = zVEC3(vertex->Position[0], vertex->Position[1], vertex->Position[2]);

						Plane vertPlane;
						Plane::initPlaneByVectors(vertPlane, vertPos, look);
						vertPlane.convertToHNF();
						// should always be true(?)
						if (vertPlane.isLineIntersecting(motionLine))
						{
							float lambda = vertPlane.getLambdaIntersectionValueForLine(motionLine);
							zVEC3 intersectPoint = motionLine.getPointByLambda(lambda);
							float distanceToIntersect = getDistanceOfVectors(oldPos, intersectPoint);
							float distanceToPos = getDistanceOfVectors(oldPos, pos);
							if (distanceToIntersect < distanceToPos || Float::areEqual(distanceToIntersect, distanceToPos))
							{
								if (globalLambda > lambda) {
									globalLambda = lambda;
								}
							}
						}
					}*/
				//}
			}

			// update position of the npc
			/*if (globalLambda != 0) {
				logStream << "Test: globalLambda = " << globalLambda << std::endl;
				util::logInfo(&logStream);
				pos = motionLine.getPointByLambda(globalLambda);
				hero->SetPositionWorld(pos);
			} else */
			if (intersected)
			{
				//oCNpcResetPos(hero, oldPos);
				//hero->SetPositionWorld(oldPos);
				pos = zVEC3(mat->m[0][3], mat->m[1][3], mat->m[2][3]);
				hero->SetPositionWorld(oldPos);
				//hero->Move(newPos.x, newPos.y, newPos.z);
				pos = hero->GetPosition();
				heroLevitationBean->oldXPos = pos.x;
				heroLevitationBean->oldYPos = pos.y;
				heroLevitationBean->oldZPos = pos.z;

				if (checkCollision(hero, *mat))
				{
					logStream << "Test: stuck detected!" << std::endl;
					util::logInfo(&logStream);
					zVEC3 normal = getCollidingPolyNormal(hero, *mat);
					//hero->SetPositionWorld(oldPos + normal*20);
				} 
				pos = hero->GetPosition();
				heroLevitationBean->oldXPos = pos.x;
				heroLevitationBean->oldYPos = pos.y;
				heroLevitationBean->oldZPos = pos.z;
				// recursive test for avoiding wall sticking
				//Test(hero);
			}
		}
	}

	/*zVEC3 headPos = hero->GetPositionWorld();
	zVEC3 testDir = headPos;
	testDir.y += 100000000;

	zCWorld* world = oCGame::GetGame()->GetWorld();
	int flags = (1 << 0) | (1 << 7) | (1 << 9) | (1 << 10) | (1 << 14);
	LevitationBean::zCWorldTraceRayNearestHit(world, headPos, testDir, nullptr, flags);
	zVEC3* intersection = &(world->foundIntersection);
	float distance = intersection->y - headPos.y;
	if (distance < 0) return;

	if (distance < abs(yUpExt - pos.y + mat->m[1][3])) {
		logStream << "Collision occured!" << std::endl;
		util::debug(&logStream);
		zVEC3 dir = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
		hero->SetPositionWorld(zVEC3(pos.x - dir.x * 20, pos.y - dir.y * 20, pos.z - dir.z * 20));
	}*/ //TODO!!!!!

	// update old position and old distance to ground of the heroLevitationBean
	heroLevitationBean->setOldPosition(hero->GetPosition());
	heroLevitationBean->setOldDistanceToGround(heroLevitationBean->getDistanceToGround(hero->GetPosition()));

};

void __cdecl zCVobCheckAutoUnlinkHook(zCVob* vob) {
	oCNpc* hero = oCNpc::GetHero();
	if (hero != NULL && (vob == hero) && Levitation::adjustHeroPosition) {
		//return;
	}
	zCVobCheckAutoUnlink(vob);
};

std::ostream & operator<<(std::ostream & os, const Plane & p)
{
	os << "(" << std::to_string(p.a) << ", " << std::to_string(p.b) << ", " << std::to_string(p.c) << ", "<< 
		std::to_string(p.d) << ")";
	return os;
}

__int32 __cdecl sysGetTime() {
	XCALL(0x00505280);
};

int __thiscall zCMaterial::IsPortalMaterial(void* pThis)
{
	XCALL(0x005648D0);
};

const float Float::EPSILON = 0.001f;