// Copyright Adam Grodzki All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"

#include "AGRDebuggerSettings.generated.h"

USTRUCT()
struct FAGRDebuggerSettingsKey
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category="Input")
    FKey InputKey;

    /** Does this binding require the control key on PC or the command key on Mac to be held */
    UPROPERTY(EditAnywhere, Category="Input")
    bool bControl = false;

    /** Does this binding require the alt key to be held */
    UPROPERTY(EditAnywhere, Category="Input")
    bool bAlt = false;

    /** Does this binding require the shift key to be held */
    UPROPERTY(EditAnywhere, Category="Input")
    bool bShift = false;

    /** Does this binding require the windows key on PC or the control key on Mac to be held */
    UPROPERTY(EditAnywhere, Category="Input")
    bool bCommand = false;

    FAGRDebuggerSettingsKey(
        const FKey& InputKey,
        const bool bControl,
        const bool bAlt,
        const bool bShift,
        const bool bCommand)
        : InputKey(InputKey),
          bControl(bControl),
          bAlt(bAlt),
          bShift(bShift),
          bCommand(bCommand)
    {
    }

    explicit FAGRDebuggerSettingsKey(const FKey& InputKey)
        : InputKey(InputKey)
    {
    }

    FAGRDebuggerSettingsKey()
    {
    }

    /**
    * Gets a localized string that represents the input key (based on InputChord implementation).
    *
    * @return A localized string.
    */
    FText GetInputText() const;
};

/**
 * Settings for AGR Debugger
 */
UCLASS(Config="Game", defaultconfig, meta=(DisplayName="AGR Debugger"))
class AGRPRO_API UAGRDebuggerSettings : public UObject
{
    GENERATED_BODY()

public:
    /** Key used to activate AGR debugger tool */
    UPROPERTY(config, EditAnywhere, Category="Input")
    FAGRDebuggerSettingsKey ActivationKey;

    /** Key used to show / hide AnimState Category panel */
    UPROPERTY(config, EditAnywhere, Category="Input")
    FAGRDebuggerSettingsKey CategoryAnimState;

    /** Key used to show / hide AimOffset Category panel */
    UPROPERTY(config, EditAnywhere, Category="Input")
    FAGRDebuggerSettingsKey CategoryAimOffset;

    /** Key used to show / hide Rotation Category panel */
    UPROPERTY(config, EditAnywhere, Category="Input")
    FAGRDebuggerSettingsKey CategoryRotation;

    /** Key used to show / hide Runtime Category panel */
    UPROPERTY(config, EditAnywhere, Category="Input")
    FAGRDebuggerSettingsKey CategoryRuntime;

    /** Key used to show / hide Movement Category panel */
    UPROPERTY(config, EditAnywhere, Category="Input")
    FAGRDebuggerSettingsKey CategoryMovement;

    /** Key used to show / hide State Category panel */
    UPROPERTY(config, EditAnywhere, Category="Input")
    FAGRDebuggerSettingsKey CategoryState;

    /** Key used to show / hide Leans Category panel */
    UPROPERTY(config, EditAnywhere, Category="Input")
    FAGRDebuggerSettingsKey CategoryLeans;

    /** Key used to show / hide Setup Category panel */
    UPROPERTY(config, EditAnywhere, Category="Input")
    FAGRDebuggerSettingsKey CategorySetup;

    /** Key used to show / hide Custom Category panel */
    UPROPERTY(config, EditAnywhere, Category="Input")
    FAGRDebuggerSettingsKey CategoryCustom;

public:
    UAGRDebuggerSettings();
};