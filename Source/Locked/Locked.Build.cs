// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Locked : ModuleRules
{
	public Locked(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "AIModule", "Slate", "SlateCore", "Niagara" });
	}
}
