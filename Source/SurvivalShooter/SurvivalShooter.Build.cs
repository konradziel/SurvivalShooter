// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SurvivalShooter : ModuleRules
{
	public SurvivalShooter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "EnhancedInput", "Core", "CoreUObject", "Engine", "InputCore", "Niagara", "AIModule"});

		PrivateDependencyModuleNames.AddRange(new string[] { "NavigationSystem"});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
