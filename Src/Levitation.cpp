#include <Levitation.h>
#include <HookManager.h>
#include <api/g2/zcworld.h>
#include <api/g2/zcinput.h>
#include <api/g2/zcmodel.h>
#include <api/g2/ocgame.h>
#include <LevitationBean.h>
#include <Util.h>
#include <queue>

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
	RotateLocalY rotateLocalY = (RotateLocalY)0x0061B720;
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
	oCNpc* hero = oCNpc::GetHero();
	if (!heroLevitationBeanCalled) {
		heroLevitationBean = new LevitationBean(hero);
		heroLevitationBeanCalled = true;
	}
	bool adjust = (hero == pThis) && adjustHeroPosition;
	float oldYPosition = 0;
	zVEC3 oldLook;
	if (adjust) {
		oldYPosition = hero->GetPosition().y;
		zMAT4* mat = &hero->trafoObjToWorld;
		oldLook = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
		heroLevitationBean->oldLook = oldLook;
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
		noCollision = false;
		zCVobSetCollDetHook(hero, 1);

		//hero->ResetXZRotationsWorld();
		zCVobSetPhysicsEnabled(hero, true);
		noCollision = true;

		zVEC3 pos = hero->GetPosition();
		if (pos.y < oldYPosition) {
			hero->SetPositionWorld(zVEC3(pos.x, oldYPosition, pos.z));
		}

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
		noCollision = false;
		Test(hero);
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
	if (hero != NULL && (pThis == hero) && adjustHeroPosition) {
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
	if (result && hero != NULL && (pThis == hero) && adjustHeroPosition) {
		logStream << "zCAIPlayerCheckEnoughSpaceMoveDirHook: called!"  << std::endl;
		util::logInfo(&logStream);
		return 0;
	}

	return result;
}

// TODO: Floor aligning doesn't work properly on certain cases; keyword: consider poly normals!
void doFloorAligning(zVEC3* finalPosition, zMAT4* mat)
{
	zVEC3 test = *finalPosition;
	zVEC3 look(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
	test += look * 20;
	float collisionYDownDist1 = getCollideYDir(test, 100, false).distance;
	float collisionYDownDist2 = getCollideYDir(*finalPosition, 100, false).distance;
	float collisionYDownDist = min(collisionYDownDist1, collisionYDownDist2);
	if (collisionYDownDist < 150)
	{
		finalPosition->y += 50 - (collisionYDownDist - 100);
	}
};

void levitate() {
	if (!Levitation::adjustHeroPosition) return;
	if (Levitation::pausedGame) return;

	std::stringstream logStream;

	oCNpc* hero = oCNpc::GetHero();
	if (oCNpcIsMovLock(hero)) return;

	float angle = 0;
	zVEC3 positionAdd(0,0,0);

	Levitation::noCollision = true;
	float speed = 100.0f; //100cm per second
	float distance = speed * float(frameTimePast) / 1000;
	zVEC3 pos = hero->GetPositionWorld();
	zVEC3 oldLook = heroLevitationBean->oldLook;
	zCInput* input = zCInput::GetInput();

	zCModelCalcModelBBox3DWorld(hero->GetModel());// ->CalcModelBBox3DWorld();
	zTBBox3D bBox = LevitationBean::zCModelGetBBox3D(hero->GetModel());// ->GetBBox3D();
	zMAT4* mat = &(hero->trafoObjToWorld);
	float yUpExt = bBox.bbox3D_maxs.y;
	float yDownExt = bBox.bbox3D_mins.y;

	hero->ResetXZRotationsLocal();
	int arrowLeft = input->KeyPressed(203);
	int arrowRight = input->KeyPressed(205);
	int arrowUp = input->KeyPressed(200);
	int moveUp = input->KeyPressed(0x1A);
	int moveDown = input->KeyPressed(0x28);

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
		angle += speed * float(frameTimePast) / 1000;
	}
	else if (arrowRight)
	{
		float speed = 90.0;
		angle += speed * float(frameTimePast) / 1000;
	}

	// Pressed � (levitation up)
	if (moveUp) {
		float collisionYUpDist = getCollideYDir(hero->GetVobPosition(), abs(yUpExt), true).distance;
		//if (collisionYUpDist > COLLISION_HEAD_SIZE_Y) {
			positionAdd.y += distance;
		//}
	}
	// Pressed � (levitation down)
	else if (moveDown) {
		//if (collisionYDownDist > abs(yDownExt / 6)) {
			positionAdd.y -= distance;
		//}
	}

	// Pressed Arrow_Up (levitation forward)
	if (input->KeyPressed(0xC8)) {
		zVEC3 look = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
		positionAdd.x += look.x * 5;
		positionAdd.y += look.y * 5;
		positionAdd.z += look.z * 5;
	}
	// Pressed Arrow_Down (levitation backward)
	else if (input->KeyPressed(0xD0)) {
		//Motion motion = getCollideForwardDir(hero, 0, true);
	}

	//finally apply changes
	rotateLocalY(hero, angle);
	zVEC3 finalPosition = hero->GetPositionWorld();
	finalPosition += positionAdd;
	if (!moveDown && !moveUp)
	{
		// TODO: Floor aligning doesn't work properly on certain cases; keyword: consider poly normals!
		//doFloorAligning(&finalPosition, mat);
	}
	hero->SetPositionWorld(finalPosition);
	heroLevitationBean->setHoverDistance(heroLevitationBean->getDistanceToGround(finalPosition));
};

Motion getCollideYDir(zVEC3 pos, float middlePointDist, bool up) {
	zVEC3 dir;
	if (up) {
		pos.y += middlePointDist;
		dir = pos;
		dir.y += 1000000000;
	}
	else {
		pos.y += middlePointDist;
		dir = pos;
		dir.y -= 1000000000;
	}
	zCWorld* world = oCGame::GetGame()->GetWorld();
	int flags = (1 << 0) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 14);
	LevitationBean::zCWorldTraceRayNearestHit(world, pos, dir, NULL, flags);
	zVEC3* intersection = &(world->foundIntersection);

	Motion motion;
	motion.normal = world->foundPolyNormal;
	motion.distance = abs(intersection->y - pos.y);
	motion.objectPos = pos;
	motion.intersect = *intersection;
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
	LevitationBean::zCWorldTraceRayNearestHit(world, pos, posOutside, NULL, flags);
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
	if (polys != NULL) {
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
	rec.bbox3D_maxs = box.bbox3D_maxs;//zVEC3(100,100,100);
	box.bbox3D_maxs.x = 20;
	box.bbox3D_maxs.z *= 0.5;
	rec.bbox3D_mins = box.bbox3D_mins;//zVEC3(-100, -100, -100);
	box.bbox3D_mins.x = -20;
	box.bbox3D_mins.z *= 0.5;
	rec.bbox3D_maxs += pos;
	rec.bbox3D_mins += pos;

	box.bbox3D_maxs += pos;
	box.bbox3D_mins += pos;

	Levitation::zCBspBaseCollectPolysInBBox3D(pointer, rec, polys, third);
	if (polys != NULL) {
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

				if (poly->CheckBBoxPolyIntersection(box))
				{
					if (!intersected) {
						intersected = true;

						//we have found what we wanted -> a collision!
						break;
					}
				}
			}
		}
	}

	return intersected;
}


void Test(oCNpc* hero) {

	std::stringstream logStream;
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
	bool upKeyPressed = input->KeyPressed(0x1A) != 0; // ü = 0x1A
	bool downKeyPressed = input->KeyPressed(0x28) != 0; // ä = 0x1A
	heroLevitationBean->setMoveDown(false);
	float distanceToGround = heroLevitationBean->getDistanceToGround(pos);
	heroLevitationBean->update(distanceToGround);
	if (yIntersect < heroLevitationBean->HOVER_DISTANCE && !upKeyPressed) {
		//TODO : check status of previously initialized LevitationBean!
		float hoverDistance = heroLevitationBean->getHoverDistance();
		if (distanceToGround < hoverDistance)
		{
		}
	}
	else if (yIntersect < 120 && !upKeyPressed) {
		//heroLevitationBean->setMoveDown(true);
	}

	zCModel* model = hero->GetModel();
	zCModelCalcModelBBox3DWorld(model);
	zVEC3 oldPos = zVEC3(heroLevitationBean->oldXPos, heroLevitationBean->oldYPos, heroLevitationBean->oldZPos);
	if (checkCollision(hero, *mat))
	{
		pos = zVEC3(mat->m[0][3], mat->m[1][3], mat->m[2][3]);
		hero->SetPositionWorld(oldPos);
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
	}

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
	os << "(" << p.a << ", " << p.b << ", " << p.c << ", "<< 
		p.d << ")";
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