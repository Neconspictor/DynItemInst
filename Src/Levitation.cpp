/*////////////////////////////////////////////////////////////////////////////

This file is part of neclib.

Copyright © 2015-2020 David Goeth

All Rights reserved.

THE WORK (AS DEFINED BELOW) IS PROVIDED
UNDER THE TERMS OF THIS CREATIVE COMMONS
PUBLIC LICENSE ("CCPL" OR "LICENSE").
THE WORK IS PROTECTED BY COPYRIGHT AND/OR
OTHER APPLICABLE LAW. ANY USE OF THE WORK
OTHER THAN AS AUTHORIZED UNDER THIS LICENSE
OR COPYRIGHT LAW IS PROHIBITED.

BY EXERCISING ANY RIGHTS TO THE WORK PROVIDED
HERE, YOU ACCEPT AND AGREE TO BE BOUND BY THE
TERMS OF THIS LICENSE. TO THE EXTENT THIS
LICENSE MAY BE CONSIDERED TO BE A CONTRACT,
THE LICENSOR GRANTS YOU THE RIGHTS CONTAINED
HERE IN CONSIDERATION OF YOUR ACCEPTANCE OF
SUCH TERMS AND CONDITIONS.

Full license at http://creativecommons.org/licenses/by-nc/3.0/legalcode

/////////////////////////////////////////////////////////////////////////////**/

#include <Levitation.h>
#include <HookManager.h>
#include <api/g2/zcworld.h>
#include <api/g2/zcinput.h>
#include <api/g2/zcmodel.h>
#include <api/g2/ocgame.h>
#include <LevitationData.h>
#include <Util.h>
#include <queue>
#include <api/g2/zcparser.h>
#include <api/g2/oCItemExtended.h>
#include <MathUtil.h>
#include <api/g2/zcpolygon.h>
#include <api/g2/ztypes.h>


bool Levitation::gameIsPaused = false;
bool Levitation::noCollision = false;
int Levitation::frameTime = 0;
int Levitation::diffFrameTime = 0;
bool Levitation::mCustomCollisionDetected = false;

LevitationData heroLevitationBean;
bool heroLevitationBeanCalled;
bool fakeUpKey;
int frameTimePast;



Levitation::Levitation() :Module("Levitation")
{
}

Levitation::~Levitation()
{
}

void Levitation::hookModule()
{
	HookManager* hookManager = HookManager::getHookManager();
	hookManager->addFunctionHook((LPVOID*)&zCVobDoFrameActivity, zCVobDoFrameActivityHook, mModuleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCGamePause, oCGamePauseHook, mModuleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCGameUnpause, oCGameUnpauseHook, mModuleDesc);
	//hookManager->addFunctionHook((LPVOID*)&zCVobCheckAndResolveCollisions, zCVobCheckAndResolveCollisionsHook, mModuleDesc);
	
	hookManager->addFunctionHook((LPVOID*)&zCVobUpdatePhysics, zCVobUpdatePhysicsHook, mModuleDesc);
	hookManager->addFunctionHook((LPVOID*)&oCAIHumanPC_ActionMove, oCAIHumanPC_ActionMoveHook, mModuleDesc);
}

void Levitation::unHookModule()
{
	HookManager* hookManager = HookManager::getHookManager();
	hookManager->removeFunctionHook((LPVOID*)&zCVobDoFrameActivity, zCVobDoFrameActivityHook, mModuleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCGamePause, oCGamePauseHook, mModuleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCGameUnpause, oCGameUnpauseHook, mModuleDesc);
	hookManager->removeFunctionHook((LPVOID*)&zCVobUpdatePhysics, zCVobUpdatePhysicsHook, mModuleDesc);
	hookManager->removeFunctionHook((LPVOID*)&oCAIHumanPC_ActionMove, oCAIHumanPC_ActionMoveHook, mModuleDesc);
	//hookManager->removeFunctionHook((LPVOID*)&zCVobCheckAndResolveCollisions, zCVobCheckAndResolveCollisionsHook, mModuleDesc);
}

zVEC3 levitatePosition;
bool doHardTests = false;

int Levitation::LEVITATION_IsGamePaused()
{
	return gameIsPaused;
}

void Levitation::zCVobDoFrameActivityHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	bool adjust = false;
	float oldYPosition = 0;
	zVEC3 oldLook;
	zVEC3 pos;


	if (pThis == hero)
	{
		frameTimePast = calcPastFrameTime(); 
		adjust = isLevitationActive();
	}

	if (adjust) {
		removeModelGravity(hero->GetModel());

		zMAT4* mat = &hero->trafoObjToWorld;
		oldLook = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
		heroLevitationBean.oldLook = oldLook;

		//check if hard collision tests should be applied (important for vobs and mobs)
		//doHardTests = check_prePass(hero, hero->trafoObjToWorld);

		//.text:0065F790 ; public: static class oCInformationManager & __cdecl oCInformationManager::GetInformationManager(void)
		using OCInformationManagerGetInformationManager = void* (__cdecl*)();
		static OCInformationManagerGetInformationManager oCInformationManagerGetInformationManager = (OCInformationManagerGetInformationManager)0x0065F790;
		//.text:006609D0 ; public: int __fastcall oCInformationManager::HasFinished(void)
		using OCInformationManagerHasFinished = int(__fastcall*)(void* pThis);
		static OCInformationManagerHasFinished oCInformationManagerHasFinished = (OCInformationManagerHasFinished)0x006609D0;
		

		void* infoManager = oCInformationManagerGetInformationManager();
		int hasFinished = oCInformationManagerHasFinished(infoManager);

		if (hasFinished)
			levitatePosition = levitate();

		
		hero->GetPositionWorld(pos.x, pos.y, pos.z);
		float hoverDistance = heroLevitationBean.getDistanceToGround(pos);
		if (true) {
			float add = getMinHoverDistance() - hoverDistance;
			pos.y += max(0, add);
			hero->SetPositionWorld(pos);
		}
		
	}

	//if (adjust) {
	//	adjust = !doCustomCollisionCheck(hero);
	//}
	
	zCVobDoFrameActivity(pThis);

	if (adjust) {
		doCustomCollisionCheck(hero);
		//hero->SetPositionWorld(pos);
	}
	

	
}

void Levitation::oCGamePauseHook(void* pThis, int second)
{
	mLogStream << "Paused Game!" << std::endl;
	util::logAlways(&mLogStream);
	Levitation::gameIsPaused = true;
	oCGamePause(pThis, second);
}

void Levitation::oCGameUnpauseHook(void* pThis)
{
	mLogStream << "Unpaused Game!" << std::endl;
	util::logAlways(&mLogStream);
	Levitation::gameIsPaused = false;
	oCGameUnpause(pThis);
}

void Levitation::zCVobCheckAndResolveCollisionsHook(void* pThis)
{

	oCNpc* hero = oCNpc::GetHero();
	void* collisionObject = nullptr;
	zVEC3 translation;

	if (hero != nullptr)
		collisionObject = hero->m_poCollisionObject;

	bool adjust = (pThis == hero) && isLevitationActive() && (collisionObject != nullptr) && !customCollisionDetected();
	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);
		translation = zVEC3(mat->_14, mat->_24, mat->_34);
		/*mLogStream << "zCVobCheckAndResolveCollisionsHook (before):" << std::endl;
		mLogStream << "\ttranslation = " << translation << std::endl;
		util::logWarning(&mLogStream);*/
		//return;
		
	}
	zCVobCheckAndResolveCollisions(pThis);

	if (adjust)
	{
		collisionObject = hero->m_poCollisionObject;
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);

		zVEC3 diff; 
		diff.x = mat->_14 - translation.x;
		diff.y = mat->_24 - translation.y;
		diff.z = mat->_34 - translation.z;
		float length = std::sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

		

		if (length < 2.2 || length > 20) return;

		mat->_14 = translation.x;
		mat->_24 = translation.y;
		mat->_34 = translation.z;
		
		/*zVEC3 translation(mat->_14, mat->_24, mat->_34);
		mLogStream << "zCVobCheckAndResolveCollisionsHook (after):" << std::endl;
		mLogStream << "\ttranslation = " << translation << std::endl;
		util::logWarning(&mLogStream);*/
	}
}

void Levitation::zCVobUpdatePhysicsHook(void* pThis)
{
	oCNpc* hero = oCNpc::GetHero();
	void* collisionObject = nullptr;

	if (hero != nullptr)
		collisionObject = hero->m_poCollisionObject;

	zVEC3 translationBefore;

	bool adjust = (pThis == hero) && isLevitationActive() && (collisionObject != nullptr);
	if (adjust)
	{
		zMAT4* mat = (zMAT4*)((char*)collisionObject + 0x44);

		auto pos = hero->GetPosition();

		mat->_14 = pos.x;
		mat->_24 = pos.y;
		mat->_34 = pos.z;
	}
	else {
		zCVobUpdatePhysics(pThis);
	}
}

int Levitation::zCVobIsColliding(void* pThis)
{
	XCALL(ZCVOB_IS_COLLIDING);
}

int Levitation::zCBspBaseCollectPolysInBBox3D(void*, zTBBox3D const&, zCPolygon**&, int&)
{
	XCALL(ZCBSPBASE_COLLECT_POLYS_IN_BBOX3D);
}

void Levitation::zCBspBaseCollectVobsInBBox3D(void* pThis, zCArray<zCVob*>& collectedVobs, zTBBox3D const& boundingBox)
{
	XCALL(0x00531110);
}

int Levitation::oCAIHumanPC_ActionMoveHook(void* pThis, int param1)
{
	void* hero = (void*)oCNpc::GetHero();

	bool adjust = isLevitationActive() && pThis != nullptr;

	if (adjust)
	{
		void* npc = *(void**)((char*)pThis + 0x12C);
		adjust &= (npc == hero);
	}

	if (adjust)
	{
		zCModel* modelAddress = *((zCModel**)((char*)pThis + 0x68));

		if (modelAddress != nullptr)
		{
			removeModelGravity(modelAddress);
		}
	}

	return oCAIHumanPC_ActionMove(pThis, param1);
}

bool Levitation::isLevitationActive()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("_LEVITATION_IsActive");
	return static_cast<bool>(symbol->content.data_int);
}

int Levitation::getMinHoverDistance()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_HOVER_DISTANCE");
	return symbol->content.data_int;
}

float Levitation::getSpeedVertical()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_SPEED_VERTICAL");
	return static_cast<float>(symbol->content.data_int);
}

float Levitation::getSpeedForward()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_SPEED_FORWARD");
	return static_cast<float>(symbol->content.data_int);
}

float Levitation::getSpeedBackward()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_SPEED_BACKWARD");
	return static_cast<float>(symbol->content.data_int);
}

float Levitation::getGravity()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_GRAVITY");
	float gravity = static_cast<float>(symbol->content.data_int);
	if (gravity < 0.0) gravity = 0.0f;
	return gravity;
}

float Levitation::getTurnSpeed()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_SPEED_TURN");
	float speedTurn = static_cast<float>(symbol->content.data_int);
	if (speedTurn < 0.0) speedTurn = 0.0f;
	return speedTurn;
}

int Levitation::getMoveForwardKey()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_CONTROL_FORWARD");
	return symbol->content.data_int;
}

int Levitation::getMoveBackwardKey()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_CONTROL_BACKWARD");
	return symbol->content.data_int;
}

int Levitation::getTurnLeftKey()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_CONTROL_LEFT");
	return symbol->content.data_int;
}

int Levitation::getTurnRightKey()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_CONTROL_RIGHT");
	return symbol->content.data_int;
}

int Levitation::getMoveUpKey()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_CONTROL_UP");
	return symbol->content.data_int;
}

int Levitation::getMoveDownKey()
{
	zCPar_Symbol* symbol = UTIL_GET_SYMBOL_WITH_CHECKS("LEVITATION_CONTROL_DOWN");
	return symbol->content.data_int;
}

bool Levitation::customCollisionDetected()
{
	return mCustomCollisionDetected;
}

void Levitation::removeModelGravity(zCModel* model)
{
	// Unfortunately i don't anymore if this is deactivating gravity or smoething else
	// But it worked, so i leave it as it is :)
	char* modelPtr = (char*)model;
	modelPtr[0x1F8] = 0xD;
}

zVEC3 Levitation::levitate() {

	zVEC3 positionAdd(0, 0, 0);

	if (!isLevitationActive()) return positionAdd;
	if (Levitation::gameIsPaused) return positionAdd;

	std::stringstream mLogStream;

	oCNpc* hero = oCNpc::GetHero();
	//if (oCNpcIsMovLock(hero)) return positionAdd;

	float angle = 0;

	//Levitation::noCollision = true;
	const float speedVertical = getSpeedVertical();
	const float speedForward = getSpeedForward();
	const float speedBackward = getSpeedBackward();
	const float distanceVertical = speedVertical * float(frameTimePast) / 1000.0f;
	const float distanceForward = speedForward * float(frameTimePast) / 1000.0f;
	const float distanceBackward = speedBackward * float(frameTimePast) / 1000.0f;
	const float distanceGravity = getGravity() * float(frameTimePast) / 1000.0f;
	const float speedTurn = getTurnSpeed();

	const int moveForwardKey = getMoveForwardKey();
	const int moveBackwardKey = getMoveBackwardKey();
	const int turnLeftKey = getTurnLeftKey();
	const int turnRightKey = getTurnRightKey();
	const int moveUpKey = getMoveUpKey();
	const int moveDownKey = getMoveDownKey();

	zVEC3 oldLook = heroLevitationBean.oldLook;
	zCInput* input = zCInput::GetInput();
	hero->ResetXZRotationsLocal();

	const auto turnLeft = input->KeyPressed(turnLeftKey);
	const auto turnRight = input->KeyPressed(turnRightKey);
	const auto moveForward = input->KeyPressed(moveForwardKey);
	const auto moveBackward = input->KeyPressed(moveBackwardKey);
	const auto moveUp = input->KeyPressed(moveUpKey);
	const auto moveDown = input->KeyPressed(moveDownKey);

	if (!turnLeft && !turnRight)
	{
		zMAT4* mat = &hero->trafoObjToWorld;
		mat->m[0][2] = oldLook.x;
		mat->m[1][2] = oldLook.y;
		mat->m[2][2] = oldLook.z;
	}

	if (turnLeft)
	{
		angle += -speedTurn * float(frameTimePast) / 1000.0f;
	}
	else if (turnRight)
	{
		angle += speedTurn * float(frameTimePast) / 1000.0f;
	}

	// Pressed � (levitation up)
	if (moveUp) {
		positionAdd.y += distanceVertical;
	}
	// Pressed � (levitation down)
	else if (moveDown) {
		positionAdd.y -= distanceVertical;
	}

	positionAdd.y -= distanceGravity;

	// Pressed Arrow_Up (levitation forward)
	if (moveForward) {
		zMAT4* mat = &hero->trafoObjToWorld;
		zVEC3 look = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
		positionAdd.x += look.x * distanceForward;
		positionAdd.y += look.y * distanceForward;
		positionAdd.z += look.z * distanceForward;
	}
	// Pressed Arrow_Down (levitation backward)
	else if (moveBackward) {
		zMAT4* mat = &hero->trafoObjToWorld;
		zVEC3 look = zVEC3(mat->m[0][2], mat->m[1][2], mat->m[2][2]);
		positionAdd.x -= look.x * distanceBackward;
		positionAdd.y -= look.y * distanceBackward;
		positionAdd.z -= look.z * distanceBackward;
	}

	//finally apply changes
	zCVobRotateLocalY(hero, angle);
	zVEC3 finalPosition = hero->GetPositionWorld();
	heroLevitationBean.oldXPos = finalPosition.x;
	heroLevitationBean.oldYPos = finalPosition.y;
	heroLevitationBean.oldZPos = finalPosition.z;
	finalPosition += positionAdd;

	hero->SetPositionWorld(finalPosition);
	//heroLevitationBean->setHoverDistance(heroLevitationBean->getDistanceToGround(finalPosition));

	return positionAdd;
};

Motion Levitation::getCollideYDir(zVEC3 pos, float middlePointDist, bool up) {
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
	LevitationData::zCWorldTraceRayNearestHit(world, pos, dir, NULL, flags);
	zVEC3* intersection = &(world->foundIntersection);

	Motion motion;
	motion.normal = world->foundPolyNormal;
	motion.distance = abs(intersection->y - pos.y);
	motion.objectPos = pos;
	motion.intersect = *intersection;
	return motion;
};

Motion Levitation::getCollideForwardDir(oCNpc* npc, float middlepointDistance, bool backward) {
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
	LevitationData::zCWorldTraceRayNearestHit(world, pos, posOutside, NULL, flags);
	zVEC3* intersection = &(world->foundIntersection);

	Motion motion;
	motion.normal = world->foundPolyNormal;
	motion.distance = abs((intersection->x - pos.x) + (intersection->y - pos.y) + (intersection->z - pos.z));
	motion.objectPos = pos;
	motion.intersect = *intersection;
	return motion;
};


int Levitation::calcPastFrameTime() {
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
	zTBBox3D box = LevitationData::zCModelGetBBox3D(model);

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
			zVEC3 oldPos = zVEC3(heroLevitationBean.oldXPos, heroLevitationBean.oldYPos, heroLevitationBean.oldZPos);
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

bool Levitation::check_prePass(oCNpc* hero, const zMAT4& mat)
{
	bool intersected = false;
	zVEC3 pos(mat.m[0][3], mat.m[1][3], mat.m[2][3]);
	zCPolygon** polys;
	void* pointer = (void*)*(int*)((BYTE*)hero + 0x0b8);
	pointer = (void*)*(int*)((BYTE*)pointer + 0x1b4);
	int third = 0;

	zCModel* model = hero->GetModel();
	zCModelCalcModelBBox3DWorld(model);
	zTBBox3D box = LevitationData::zCModelGetBBox3D(model);

	//make rec a little bit bigger than box
	box.bbox3D_maxs += pos;
	box.bbox3D_mins += pos;

	zCArray<zCVob*> collectedVobs;
	Levitation::zCBspBaseCollectVobsInBBox3D(pointer, collectedVobs, box);

	return (collectedVobs.GetSize() > 1); // +1 for hero
}

bool checkVobCollision__checkVob(zCVob* vob, const zTBBox3D& boundingBox)
{
	if (vob == nullptr) return false;

	std::stringstream mLogStream;


	zCArray<void*>* leafObjects = &vob->vobLeafList;
	zCPolygon** polys;
	//int third;
	mLogStream << "checkVobCollision__checkVob(): Check object with leaf number: " << leafObjects->GetSize() << std::endl;

	auto& name = vob->objectName;

	mLogStream << "checkVobCollision__checkVob(): visual name: " << name.ToChar() << std::endl;
	util::logInfo(&mLogStream);

	for (unsigned int i = 0; i < leafObjects->GetSize(); ++i)
	{
		zCBspBase_Small* leaf = (zCBspBase_Small*)leafObjects->GetItem(i);
		//Levitation::zCBspBaseCollectPolysInBBox3D(leaf, boundingBox, polys, third);
		mLogStream << "checkVobCollision__checkVob(): leave has poly count: " << leaf->numPolys << std::endl;
		//logStream << "checkVobCollision__checkBspLeaf(): Found polys: " << third << std::endl;
		util::logInfo(&mLogStream);

		zCPolygon** polyList = (zCPolygon**)leaf->polyList;

		for (int j = 0; j < leaf->numPolys; ++j)
		{
			zCPolygon* poly = polyList[j];

			//logStream << "checkVobCollision__checkVob(): pos = " << pos << std::endl;
			//logStream << "checkVobCollision__checkVob(): vob pos = " << vob->GetPositionWorld() << std::endl;
			//util::logInfo(&logStream);
			if (poly->CheckBBoxPolyIntersection(boundingBox))
			{
				mLogStream << "checkVobCollision__checkVob(): Intersection found! " << std::endl;
				util::logInfo(&mLogStream);
				return true;
			}
		}
		
	}
	return false;
}


bool Levitation::checkVobCollision(void* zCBspBaseObject, zCVob* testedVob, const zTBBox3D& boundingBox)
{
	zCArray<zCVob*> collectedVobs;
	Levitation::zCBspBaseCollectVobsInBBox3D(zCBspBaseObject, collectedVobs, boundingBox);

	if (collectedVobs.GetSize() > 0)
	{
		std::stringstream mLogStream;
		mLogStream << "checkVobCollision(): Found vobs!: " << collectedVobs.GetSize() << std::endl;
		util::logInfo(&mLogStream);

		for (unsigned int i = 0; i < collectedVobs.GetSize(); ++i)
		{
			mLogStream << "checkVobCollision(): test vob with number: " << i << std::endl;
			util::logInfo(&mLogStream);

			if (i > 0) {
				bool test = false;
			}

			zCVob* vob = collectedVobs.GetItem(i);
			if (vob != testedVob && vob != nullptr)
			{
				int zCVob_bitfield0_collDetectionDynamic = ((1 << 1) - 1) << 7;
				int zCVob_bitfield0_staticVob = ((1 << 1) - 1) << 4;
				bool collidesWithDynamicVobs = vob->bitfield[0] & zCVob_bitfield0_collDetectionDynamic;
				bool isStaticVob = vob->bitfield[0] & zCVob_bitfield0_staticVob;
				//bool hasCollisionObject = vob->m_poCollisionObject != nullptr;
				if (collidesWithDynamicVobs) //isStaticVob
				{
					if (checkVobCollision__checkVob(vob, boundingBox))
						return true;
				}
			}
		}
	}

	return false;
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
	zTBBox3D box = LevitationData::zCModelGetBBox3D(model);

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
			zVEC3 oldPos = zVEC3(heroLevitationBean.oldXPos, heroLevitationBean.oldYPos, heroLevitationBean.oldZPos);
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
					zCMaterial* material = poly->GetMaterial();
					char* bitflag = ((char*)(char*)material + 0x70);
					bool hasNoCollision = *bitflag & (1 << 4);

					const char* materialName = material->GetName().ToChar();
					bool isGhostOccluder = strcmp(materialName, "GHOSTOCCLUDER") == 0;
					//bool isNW_MISC_FULLALPHA_01 = strcmp(materialName, "NW_MISC_FULLALPHA_01") == 0;

					if (isGhostOccluder) {
						//*bitflag = *bitflag | (1 << 4);
					}

					if (hasNoCollision)
					{
						//std::stringstream logStream;
						//logStream << "checkCollision(): ignore poly with no collision material" << std::endl;
						//util::debug(&logStream);
						continue;
					}

					if (isGhostOccluder)
					{
						//std::stringstream logStream;
						//logStream << "checkCollision(): ignore poly with GHOSTOCCLUDER material" << std::endl;
						//util::debug(&logStream);
						continue;
					}

					/*if (isNW_MISC_FULLALPHA_01)
					{
						//std::stringstream logStream;
						//logStream << "checkCollision(): ignore poly with NW_MISC_FULLALPHA_01 material" << std::endl;
						//util::debug(&logStream);
						continue;
					}*/

				

					if (!intersected && !hasNoCollision && !isGhostOccluder) {
						intersected = true;

						//std::stringstream logStream;
						//logStream << "checkCollision(): intersection found!: " << materialName << std::endl;
						//util::debug(&logStream);
						//we have found what we wanted -> a collision!
						break;
					}
				}
			}
		}
	}

	//if (!intersected)
		//intersected = Levitation::checkVobCollision(pointer, hero, rec);

	return intersected;
}


bool Levitation::doCustomCollisionCheck(oCNpc* npc) {

	std::stringstream mLogStream;
	zTBBox3D bBox = LevitationData::zCModelGetBBox3D(npc->GetModel());// ->GetBBox3D();
	zVEC3 pos = npc->GetPosition();
	zMAT4* mat = &(npc->trafoObjToWorld);
	float yUpExt = bBox.bbox3D_maxs.y;

	zCModel* model = npc->GetModel();
	zCModelCalcModelBBox3DWorld(model);
	zVEC3 oldPos = zVEC3(heroLevitationBean.oldXPos, heroLevitationBean.oldYPos, heroLevitationBean.oldZPos);

	if (mCustomCollisionDetected = checkCollision(npc, *mat))
	{
		npc->SetPositionWorld(oldPos);

		//if (checkCollision(hero, *mat))
		//{
		//	logStream << "Test: stuck detected!" << std::endl;
		//	util::logInfo(&logStream);
		//	zVEC3 normal = getCollidingPolyNormal(hero, *mat);
			//hero->SetPositionWorld(oldPos + normal*20);
		//} 
	}

	return mCustomCollisionDetected;
}

__int32 __cdecl Levitation::sysGetTime() {
	XCALL(0x00505280);
};