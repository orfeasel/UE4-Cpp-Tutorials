// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AutomationPost : ModuleRules
{
	public AutomationPost(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "UnrealEd", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
