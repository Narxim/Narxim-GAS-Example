// Copyright 2021 Adam Grodzki All Rights Reserved.

using UnrealBuildTool;

public class AGRPRO : ModuleRules {

    public AGRPRO(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
                // ... add public include paths required here ...
            }
        );


        PrivateIncludePaths.AddRange(
            new string[] {
                // ... add other private include paths required here ...
            }
        );


        PublicDependencyModuleNames.AddRange(
            new[] {
                "Core",
                "GameplayTags",
                "PhysicsCore",
                "Niagara",
                // ... add other public dependencies that you statically link with here ...
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new[] {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "InputCore",
                "UMG",
                "AnimGraphRuntime",
                // ... add private dependencies that you statically link with here ...
            }
        );


        DynamicallyLoadedModuleNames.AddRange(
            new string[] {
                // ... add any modules that your module loads dynamically here ...
            }
        );
    }

}