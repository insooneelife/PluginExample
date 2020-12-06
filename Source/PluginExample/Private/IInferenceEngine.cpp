// Fill out your copyright notice in the Description page of Project Settings.


#include "IInferenceEngine.h"
#include "PluginExample.h"
#include "Modules/ModuleManager.h"

// Add default functionality here for any IInferenceEngine functions that are not pure virtual.


IInferenceEngine* IInferenceEngine::Get(const FName& SubsystemName)
{
    static const FName ModuleName = TEXT("PluginExample");

    FPluginExampleModule& Module = FModuleManager::GetModuleChecked<FPluginExampleModule>(ModuleName);

    return Module.GetInferenceEngine();
}