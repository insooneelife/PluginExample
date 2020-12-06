// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "IInferenceEngine.h"

class FPluginExampleModule : public IModuleInterface, public IInferenceEngine
{
public:

	// IModuleInterface implementation
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

    void* SearchAndLoadDllsInDirectory(FString RootDirectoryPath, FString DllName);

    // IInferenceEngine
public:
    virtual void RunTestcase() override;
    virtual IInferenceEngine* GetInferenceEngine() override { return this; }

private:
    TArray<void*> DLLHandles;
};
