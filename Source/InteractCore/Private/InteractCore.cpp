// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractCore.h"
#include "InteractDebug.h"

#define LOCTEXT_NAMESPACE "FInteractCoreModule"

void FInteractCoreModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FInteractCoreModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FInteractCoreModule, InteractCore)

DEFINE_LOG_CATEGORY(Log_InteractCore)