// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class InteractCore : ModuleRules
{
	public InteractCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
	 			Path.Combine(ModuleDirectory, "Public"),
				Path.Combine(ModuleDirectory, "Public", "Interaction"),
				Path.Combine(ModuleDirectory, "Public", "Interactable"),
				Path.Combine(ModuleDirectory, "Public", "Interface"),
				Path.Combine(ModuleDirectory, "Public", "Widget"),
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory, "Private"),
        		Path.Combine(ModuleDirectory, "Private", "Interaction"),
        		Path.Combine(ModuleDirectory, "Private", "Interactable"),
        		Path.Combine(ModuleDirectory, "Private", "Interface"),
        		Path.Combine(ModuleDirectory, "Private", "Widget"),
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"EnhancedInput",
				"UMG"
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
