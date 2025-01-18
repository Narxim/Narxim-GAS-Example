// Copyright Adam Grodzki All Rights Reserved.

#include "UI/AGRDebuggerSettings.h"

// =============================================================================
// These includes are needed to prevent plugin build failures.
#include "Framework/Commands/InputChord.h"
// =============================================================================

FText FAGRDebuggerSettingsKey::GetInputText() const
{
    const FInputChord InputChord = FInputChord(
        InputKey,
        bShift,
        bControl,
        bAlt,
        bCommand
    );

    return InputChord.GetInputText();
}

UAGRDebuggerSettings::UAGRDebuggerSettings()
{
    // Sets default values
    ActivationKey = FAGRDebuggerSettingsKey(EKeys::Invalid);
    CategoryAnimState = FAGRDebuggerSettingsKey(EKeys::One);
    CategoryAimOffset = FAGRDebuggerSettingsKey(EKeys::Two);
    CategoryRotation = FAGRDebuggerSettingsKey(EKeys::Three);
    CategoryRuntime = FAGRDebuggerSettingsKey(EKeys::Four);
    CategoryMovement = FAGRDebuggerSettingsKey(EKeys::Five);
    CategoryState = FAGRDebuggerSettingsKey(EKeys::Six);
    CategoryLeans = FAGRDebuggerSettingsKey(EKeys::Seven);
    CategorySetup = FAGRDebuggerSettingsKey(EKeys::Eight);
    CategoryCustom = FAGRDebuggerSettingsKey(EKeys::Nine);
}