#pragma once

#include "ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(OrfeasModule, All, All);

class FOrfeasModule : public IModuleInterface
{
	public:

	/* This will get called when the editor loads the module */
	virtual void StartupModule() override;

	/* This will get called when the editor unloads the module */
	virtual void ShutdownModule() override;
};