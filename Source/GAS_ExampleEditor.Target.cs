// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class GAS_ExampleEditorTarget : TargetRules
{
	public GAS_ExampleEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		
		bOverrideBuildEnvironment = true;
		bWithPushModel = true;

		// , "UE5Coro" 
		ExtraModuleNames.AddRange( new string[] { "GAS_Example"} );
		
		// EnablePlugins.Add("ue5coro");
	}
}
