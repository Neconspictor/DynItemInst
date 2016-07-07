#pragma once

#include "Module.h"

class PortalDrawDistance : public Module
{
public:

	/**
	* Default constructor
	*/
	PortalDrawDistance();

	/**
	* Default virtual destructor.
	*/
	virtual ~PortalDrawDistance();

	/*! @copydoc Module::hookModule()
					*/
	virtual void hookModule() override;

	/*! @copydoc Module::unHookModule()
	*/
	virtual void unHookModule() override;

private:
	void changeOutDoorPortalDistanceMulitplier1();
	void changeOutDoorPortalDistanceMulitplier2();
	void changeInDoorPortalDistanceMultiplier1();
	void changeInDoorPortalDistanceMultiplier2();

};
