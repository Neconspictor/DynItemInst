#pragma once
#include "Module.h"
#include "api/g2/zcvob.h";
#include <list>


class Vector2D
{
public:
	Vector2D();
	Vector2D(float, float);
	virtual ~Vector2D();

	float squareLength() const;
	float cross(const Vector2D&) const;
	float dot(const Vector2D&) const;
	
	static float angleBetween(const Vector2D&, const Vector2D&);

private:
	float x, y;
};


class TestModule : public Module
{
public:
	TestModule();
	virtual ~TestModule();

	virtual void hookModule() override;
	virtual void unHookModule() override;

	static void Test();
	static void Test(zCVob* vob);

	static void __thiscall zCVobDoFrameActivityHook(void* pThis);

	static std::list<zCVob*> vobList;
};
