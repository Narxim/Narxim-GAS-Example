// Copyright 2021 Adam Grodzki All Rights Reserved.

#include "AGRPRO.h"
#include "Modules/ModuleManager.h"

// =============================================================================
// These includes need to be specified using full paths to avoid packaging
// errors.
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsSection.h"
#include "Developer/Settings/Public/ISettingsContainer.h"
// =============================================================================

#include "UI/AGRDebuggerSettings.h"

#define LOCTEXT_NAMESPACE "FAGRPROModule"

void FAGRPROModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

    // Register custom settings to appear in the project settings
    if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->RegisterSettings(
            "Project",
            "Plugins",
            "AGRDebugger",
            LOCTEXT("AGRDebuggerName", "AGR Debugger"),
            LOCTEXT(
                "AGRDebuggerDesc",
                "Configure the AGR Debugger and customize inputs for Debug Widget display and category hide / show"),
            GetMutableDefault<UAGRDebuggerSettings>()
        );
    }
}

void FAGRPROModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.

    // Remove custom settings
    if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->UnregisterSettings("Project", "Plugins", "AGRDebugger");
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAGRPROModule, AGRPRO)