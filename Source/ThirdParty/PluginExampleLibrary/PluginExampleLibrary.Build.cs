// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class PluginExampleLibrary : ModuleRules
{
	public PluginExampleLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// Add the import library
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "x64", "Release", "ExampleLibrary.lib"));

			string LibraryDirectory = Path.Combine(ModuleDirectory, "onnxruntime", "Lib", "win-x64");

			// Add the import library
			PublicAdditionalLibraries.Add(Path.Combine(LibraryDirectory, "onnxruntime.lib"));

			PublicIncludePaths.AddRange( 
				new string[] {
					Path.Combine(ModuleDirectory, "onnxruntime", "Include")
			});

			string OutDirectory = "$(BinaryOutputDir)";
			string ThirdPartyDLLName = "ExampleLibrary.dll";
			string ThirdPartyDirectory = Path.Combine(ModuleDirectory, "x64", "Release");

			// Delay-load the DLL, so we can load it from the right place first
			PublicDelayLoadDLLs.Add(ThirdPartyDLLName);

			// Ensure that the DLL is staged along with the executable
			RuntimeDependencies.Add(
				Path.Combine(OutDirectory, ThirdPartyDLLName),
				Path.Combine(ThirdPartyDirectory, ThirdPartyDLLName));

			string[] DLLNames =
			{
				"onnxruntime.dll"
			};
			string OnnxRuntimeBinDirectory =
				Path.Combine(ModuleDirectory, "onnxruntime", "Bin", "win-x64");

			foreach (var dll in DLLNames)
			{
				PublicDelayLoadDLLs.Add(dll);
				RuntimeDependencies.Add(
					Path.Combine(OutDirectory, dll),
					Path.Combine(OnnxRuntimeBinDirectory, dll));
			}
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libExampleLibrary.dylib"));
            RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/PluginExampleLibrary/Mac/Release/libExampleLibrary.dylib");
        }

	}
}
