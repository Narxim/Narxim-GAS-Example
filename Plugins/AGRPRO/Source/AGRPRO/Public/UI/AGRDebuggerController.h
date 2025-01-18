// Copyright Adam Grodzki All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

#include "AGRDebuggerController.generated.h"

class UAGRUWDebugWidget;
struct FAGRDebuggerSettingsKey;
class AAGRDebuggerPlayerManager;

UENUM()
enum class EAGRDebuggerCategories
{
    Custom,
    AnimState,
    AimOffset,
    Rotation,
    Runtime,
    Movement,
    State,
    Leans,
    Setup
};

/**
 * Holds Owner PlayerController and handles input binding for AGR Debugger
 */
UCLASS(NotBlueprintable, NotBlueprintType, noteditinlinenew, hidedropdown, Transient)
class AGRPRO_API UAGRDebuggerController : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY()
    UAGRUWDebugWidget* DebugWidget;

    UPROPERTY()
    APlayerController* CachedPlayerController;

#if WITH_EDITORONLY_DATA
    /** Qwerty Key to Azerty key association */
    UPROPERTY()
    TMap<FKey, FKey> InputMappings = {
        {EKeys::One, EKeys::Ampersand},
        {EKeys::Two, EKeys::E_AccentAigu},
        {EKeys::Three, EKeys::Quote},
        {EKeys::Four, EKeys::Apostrophe},
        {EKeys::Five, EKeys::LeftParantheses},
        {EKeys::Six, EKeys::Hyphen},
        {EKeys::Seven, EKeys::E_AccentGrave},
        {EKeys::Eight, EKeys::Underscore},
        {EKeys::Nine, EKeys::C_Cedille},
    };
#endif

public:
#if WITH_EDITOR
    void Initialize(APlayerController* OwnerPC);
    void BindInputs(UInputComponent& InputComponent);

    void RegisterCategory(
        UInputComponent& InputComponent,
        FAGRDebuggerSettingsKey CategoryKey,
        EAGRDebuggerCategories Category
    );

    void SetupBindKey(
        UInputComponent& InputComponent,
        const FInputChord& InputChord,
        const EAGRDebuggerCategories Category);

    void OnActivationPressed();

    void OnCustomCategoryPressed();
    void OnAnimStateCategoryPressed();
    void OnAimOffsetCategoryPressed();
    void OnRotationCategoryPressed();
    void OnRuntimeCategoryPressed();
    void OnMovementCategoryPressed();
    void OnStateCategoryPressed();
    void OnLeansCategoryPressed();
    void OnSetupCategoryPressed();

    FKey GetAzertyKeyFor(FKey InputKey);

    UUserWidget* CreateDebugWidget();
    void ToggleDebugWidget();
#endif
};