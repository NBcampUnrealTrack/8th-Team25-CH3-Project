// AntarcitcKids.Build.cs
// Copyright (c) 2026 AntarcticKids. All rights reserved.

using UnrealBuildTool;

public class AntarcitcKids : ModuleRules
{
	public AntarcitcKids(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.Add(ModuleDirectory);
	
		PublicDependencyModuleNames.AddRange(new string[]
		{

			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "ChaosVehicles", "Landscape"

		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
		PrivateDependencyModuleNames.AddRange(new string[] { "Niagara","NiagaraCore" });

	}
}
