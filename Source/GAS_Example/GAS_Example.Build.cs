// Copyright 2021 Joseph "Narxim" Thigpen.

using UnrealBuildTool;

public class GAS_Example : ModuleRules
{
	public GAS_Example(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"UMG",
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"GameplayAbilities",
			"GameplayTasks",
			"GameplayTags",
			"EnhancedInput",
			"CommonUI",
			"ModelViewViewModel"
		});
	}
}