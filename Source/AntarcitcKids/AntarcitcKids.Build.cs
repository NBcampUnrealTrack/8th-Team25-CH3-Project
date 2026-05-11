// AntarcitcKids.Build.cs

using UnrealBuildTool;

public class AntarcitcKids : ModuleRules
{
	public AntarcitcKids(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.Add(ModuleDirectory);
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "ChaosVehicles"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

	}
}
