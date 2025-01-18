// Copyright Adam Grodzki All Rights Reserved.

#include "UI/AGRDebuggerController.h"

#include "AGRLog.h"
#include "UI/AGRDebuggerSettings.h"
#include "UI/AGRUWDebugWidget.h"

#if WITH_EDITOR

void UAGRDebuggerController::Initialize(APlayerController* OwnerPC)
{
    CachedPlayerController = OwnerPC;
}

void UAGRDebuggerController::BindInputs(UInputComponent& InputComponent)
{
    const UAGRDebuggerSettings* SettingsCDO = UAGRDebuggerSettings::StaticClass()->GetDefaultObject<
        UAGRDebuggerSettings>();

    const FAGRDebuggerSettingsKey ActivationKey = SettingsCDO->ActivationKey;

    if(!ActivationKey.InputKey.IsValid())
    {
        return;
    }

    const FInputChord ActivationChord = FInputChord(
        ActivationKey.InputKey,
        ActivationKey.bShift,
        ActivationKey.bControl,
        ActivationKey.bAlt,
        ActivationKey.bCommand
    );

    InputComponent.BindKey(ActivationChord, IE_Pressed, this, &UAGRDebuggerController::OnActivationPressed);

    // Register categories inputs
    RegisterCategory(InputComponent, SettingsCDO->CategoryCustom, EAGRDebuggerCategories::Custom);
    RegisterCategory(InputComponent, SettingsCDO->CategoryLeans, EAGRDebuggerCategories::Leans);
    RegisterCategory(InputComponent, SettingsCDO->CategorySetup, EAGRDebuggerCategories::Setup);
    RegisterCategory(InputComponent, SettingsCDO->CategoryMovement, EAGRDebuggerCategories::Movement);
    RegisterCategory(InputComponent, SettingsCDO->CategoryRotation, EAGRDebuggerCategories::Rotation);
    RegisterCategory(InputComponent, SettingsCDO->CategoryRuntime, EAGRDebuggerCategories::Runtime);
    RegisterCategory(InputComponent, SettingsCDO->CategoryState, EAGRDebuggerCategories::State);
    RegisterCategory(InputComponent, SettingsCDO->CategoryAimOffset, EAGRDebuggerCategories::AimOffset);
    RegisterCategory(InputComponent, SettingsCDO->CategoryAnimState, EAGRDebuggerCategories::AnimState);
}

void UAGRDebuggerController::RegisterCategory(
    UInputComponent& InputComponent,
    const FAGRDebuggerSettingsKey CategoryKey,
    const EAGRDebuggerCategories Category
)
{
    if(!CategoryKey.InputKey.IsValid())
    {
        return;
    }

    AGR_LOG(
        Verbose,
        TEXT("UAGRDebuggerController - Register Category and bind inputs for %s"),
        *CategoryKey.InputKey.ToString())
    const FInputChord InputChord = FInputChord(
        CategoryKey.InputKey,
        CategoryKey.bShift,
        CategoryKey.bControl,
        CategoryKey.bAlt,
        CategoryKey.bCommand
    );

    SetupBindKey(InputComponent, InputChord, Category);

    // Try to find an associated azerty key, and if it exists bind an additional input for azerty users
    const FKey AzertyKey = GetAzertyKeyFor(CategoryKey.InputKey);
    if(AzertyKey.IsValid())
    {
        const FInputChord AzertyInputChord = FInputChord(
            AzertyKey,
            CategoryKey.bShift,
            CategoryKey.bControl,
            CategoryKey.bAlt,
            CategoryKey.bCommand
        );

        SetupBindKey(InputComponent, AzertyInputChord, Category);
    }
}

void UAGRDebuggerController::SetupBindKey(
    UInputComponent& InputComponent,
    const FInputChord& InputChord,
    const EAGRDebuggerCategories Category
)
{
    FInputKeyBinding KB(InputChord, IE_Pressed);
    KB.bConsumeInput = false;

    switch(Category)
    {
    case EAGRDebuggerCategories::Custom:
        KB.KeyDelegate.BindDelegate(this, &ThisClass::OnCustomCategoryPressed);
        break;

    case EAGRDebuggerCategories::Leans:
        KB.KeyDelegate.BindDelegate(this, &ThisClass::OnLeansCategoryPressed);
        break;

    case EAGRDebuggerCategories::Movement:
        KB.KeyDelegate.BindDelegate(this, &ThisClass::OnMovementCategoryPressed);
        break;

    case EAGRDebuggerCategories::Rotation:
        KB.KeyDelegate.BindDelegate(this, &ThisClass::OnRotationCategoryPressed);
        break;

    case EAGRDebuggerCategories::Runtime:
        KB.KeyDelegate.BindDelegate(this, &ThisClass::OnRuntimeCategoryPressed);
        break;

    case EAGRDebuggerCategories::Setup:
        KB.KeyDelegate.BindDelegate(this, &ThisClass::OnSetupCategoryPressed);
        break;

    case EAGRDebuggerCategories::State:
        KB.KeyDelegate.BindDelegate(this, &ThisClass::OnStateCategoryPressed);
        break;

    case EAGRDebuggerCategories::AimOffset:
        KB.KeyDelegate.BindDelegate(this, &ThisClass::OnAimOffsetCategoryPressed);
        break;

    case EAGRDebuggerCategories::AnimState:
        KB.KeyDelegate.BindDelegate(this, &ThisClass::OnAnimStateCategoryPressed);
        break;

    default:
        break;
    }

    InputComponent.KeyBindings.Emplace(MoveTemp(KB));
}

void UAGRDebuggerController::OnActivationPressed()
{
    ToggleDebugWidget();
}

void UAGRDebuggerController::OnCustomCategoryPressed()
{
    if(IsValid(DebugWidget))
    {
        DebugWidget->ToggleCategory("Default");
    }
}

void UAGRDebuggerController::OnAnimStateCategoryPressed()
{
    if(IsValid(DebugWidget))
    {
        DebugWidget->ToggleCategory("AGR|Anim States");
    }
}

void UAGRDebuggerController::OnAimOffsetCategoryPressed()
{
    if(IsValid(DebugWidget))
    {
        DebugWidget->ToggleCategory("AGR|AimOffset");
    }
}

void UAGRDebuggerController::OnRotationCategoryPressed()
{
    if(IsValid(DebugWidget))
    {
        DebugWidget->ToggleCategory("AGR|Rotation");
    }
}

void UAGRDebuggerController::OnRuntimeCategoryPressed()
{
    if(IsValid(DebugWidget))
    {
        DebugWidget->ToggleCategory("AGR|Runtime");
    }
}

void UAGRDebuggerController::OnMovementCategoryPressed()
{
    if(IsValid(DebugWidget))
    {
        DebugWidget->ToggleCategory("AGR|Movement");
    }
}

void UAGRDebuggerController::OnStateCategoryPressed()
{
    if(IsValid(DebugWidget))
    {
        DebugWidget->ToggleCategory("AGR|State");
    }
}

void UAGRDebuggerController::OnLeansCategoryPressed()
{
    if(IsValid(DebugWidget))
    {
        DebugWidget->ToggleCategory("AGR|Leans");
    }
}

void UAGRDebuggerController::OnSetupCategoryPressed()
{
    if(IsValid(DebugWidget))
    {
        DebugWidget->ToggleCategory("AGR|Setup");
    }
}

FKey UAGRDebuggerController::GetAzertyKeyFor(const FKey InputKey)
{
    FKey* MatchedKeyPtr = InputMappings.Find(InputKey);
    return MatchedKeyPtr ? *MatchedKeyPtr : EKeys::Invalid;
}

UUserWidget* UAGRDebuggerController::CreateDebugWidget()
{
    if(!IsValid(CachedPlayerController))
    {
        return nullptr;
    }

    // TODO
    //
    // Quick and dirty get widget blueprint class reference, should expose that as a TSubclassOf<UAGRUWDebugWidget>
    // UPROPERTY in DeveloperSettings or AnimMasterComponent
    const TSubclassOf<UUserWidget> DebugWidgetClass = StaticLoadClass(
        UObject::StaticClass(),
        nullptr,
        TEXT("/AGRPRO/UI/WP_AGR_DebugWidget.WP_AGR_DebugWidget_C")
    );

    if(!DebugWidgetClass)
    {
        AGR_WARN(TEXT("Failed to load DebugWidgetClass, not creating widget"));
        return nullptr;
    }

    UAGRUWDebugWidget* Widget = Cast<UAGRUWDebugWidget>(CreateWidget(CachedPlayerController, DebugWidgetClass));
    if(!IsValid(Widget))
    {
        return nullptr;
    }

    DebugWidget = Widget;
    return Widget;
}

void UAGRDebuggerController::ToggleDebugWidget()
{
    if(IsValid(DebugWidget) && DebugWidget->IsVisible())
    {
        DebugWidget->RemoveFromParent();
        return;
    }

    if(!IsValid(DebugWidget))
    {
        CreateDebugWidget();
    }

    if(IsValid(DebugWidget))
    {
        DebugWidget->AddToViewport();
    }
}

#endif