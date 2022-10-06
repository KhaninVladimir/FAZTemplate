// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealTest : ModuleRules
{
	public UnrealTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject",
			"Engine", 
			"InputCore", 
			"HeadMountedDisplay",
			
			"Networking",
			"NetCore",
			
			"AIModule",
			"NavigationSystem",

			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",

			"Slate",
			"SlateCore",

			"OnlineSubsystem", 
			"OnlineSubsystemUtils",
		});

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");
	}
}
