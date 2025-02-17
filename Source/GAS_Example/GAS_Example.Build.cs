// Copyright 2021 Joseph "Narxim" Thigpen.

using System.IO;
using UnrealBuildTool;

public class GAS_Example : ModuleRules
{
	public GAS_Example(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		// Added UE5Coro, and allow to activate it. Note: You need MSVC 14.41+ (can be tricky, won't be activated by default for now)
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"UMG",
			"Slate",
			"SlateCore",
			"NetCore"
			// "UE5Coro",
			// "UE5CoroGAS"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"GameplayAbilities",
			"GameplayTasks",
			"GameplayTags",
			"EnhancedInput",
			"CommonUI",
			"ModelViewViewModel"
			// "UE5Coro",
			// "UE5CoroGAS"
		});
	}
}