// Copyright Epic Games, Inc. All Rights Reserved.

#include "PluginExample.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "PluginExampleLibrary/ExampleLibrary.h"

#define LOCTEXT_NAMESPACE "FPluginExampleModule"

void FPluginExampleModule::StartupModule()
{
    TArray<FString> DLLs =
    {
        TEXT("ExampleLibrary.dll")
    };

    for (FString DLLName : DLLs)
    {
        void* Handle  = SearchAndLoadDllsInDirectory(FPaths::ProjectPluginsDir(), DLLName);

        if (Handle != nullptr)
        {
            DLLHandles.Add(Handle);
        }
    }
}

void FPluginExampleModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

    for (void* Handle : DLLHandles)
    {
        FPlatformProcess::FreeDllHandle(Handle);
    }
    DLLHandles.Empty();
}

void* FPluginExampleModule::SearchAndLoadDllsInDirectory(FString RootDirectoryPath, FString DllName)
{
    TArray<FString> DirectoriesToSkip;
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FLocalTimestampDirectoryVisitor Visitor(PlatformFile, DirectoriesToSkip, DirectoriesToSkip, false);
    PlatformFile.IterateDirectory(*RootDirectoryPath, Visitor);

    for (TMap<FString, FDateTime>::TIterator TimestampIt(Visitor.FileTimes); TimestampIt; ++TimestampIt)
    {
        const FString File = TimestampIt.Key();
        const FString FilePath = FPaths::GetPath(File);
        const FString FileName = FPaths::GetCleanFilename(File);

        if (FileName.Compare(DllName) == 0)
        {
            FPlatformProcess::AddDllDirectory(*FilePath);

            return FPlatformProcess::GetDllHandle(*File);
        }
    }

    return nullptr;
}


void FPluginExampleModule::RunTestcase()
{
    FString ModelPath = FPaths::Combine(
        FPaths::ProjectPluginsDir(),
        FString(TEXT("PluginExample")),
        FString(TEXT("Extras")),
        FString(TEXT("models")),
        FString(TEXT("model.onnx"))
    );

    std::wstring val = ExampleLibraryFunction(*ModelPath);
    FString Msg = val.c_str();

    UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPluginExampleModule, PluginExample)
