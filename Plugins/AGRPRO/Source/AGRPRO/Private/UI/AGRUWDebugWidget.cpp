// Copyright Adam Grodzki All Rights Reserved.


#include "UI/AGRUWDebugWidget.h"

#include "AGRLog.h"
#include "Components/AGRAnimMasterComponent.h"
#include "Animation/AGRCoreAnimInstance.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameFramework/Character.h"
#include "UI/AGRDebuggerSettings.h"

#if WITH_EDITOR

void UAGRUWDebugWidget::SetupOwner(APlayerController* OwningPlayer)
{
    if(!IsValid(OwningPlayer))
    {
        AGR_ERROR(TEXT("UAGRUWDebugWidget::SetupOwner passed in PlayerController is null"))
        return;
    }

    OwnerPlayerController = OwningPlayer;
    OwnerCharacter = Cast<ACharacter>(OwningPlayer->GetPawn());
    if(!OwnerCharacter.IsValid())
    {
        AGR_ERROR(TEXT("UAGRUWDebugWidget::SetupOwner OwnerPawn is not a Character"))
        return;
    }

    OwnerAnimMaster = OwnerCharacter->FindComponentByClass<UAGRAnimMasterComponent>();
    if(!OwnerAnimMaster.IsValid())
    {
        AGR_ERROR(TEXT("UAGRUWDebugWidget::SetupOwner Couldn't retrieve AGRAnimMasterComponent from Character"))
        return;
    }

    OwnerAnimInstance = Cast<UAGRCoreAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
    if(!OwnerAnimInstance.IsValid())
    {
        AGR_ERROR(TEXT("UAGRUWDebugWidget::SetupOwner OwnerAnim instance doesn't seem an AGRCoreAnimInstance"))
        return;
    }

    AnimClass = OwnerAnimInstance->GetClass();
}

void UAGRUWDebugWidget::ToggleCategory(const FString Category)
{
    for(FAGRPropContainer PropContainer : PropsContainers)
    {
        if(PropContainer.Category == Category)
        {
            UVerticalBox* Container = PropContainer.Container;
            Container->SetVisibility(Container->IsVisible() ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
        }
    }
}

void UAGRUWDebugWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SetupOwner(GetOwningPlayer());

    const UAGRDebuggerSettings* SettingsCDO = UAGRDebuggerSettings::StaticClass()->GetDefaultObject<
        UAGRDebuggerSettings>();

    if(bPropsInitialized)
    {
        return;
    }

    AGR_LOG(Log, TEXT("UAGRUWDebugWidget CONSTRUCT ---------- Props -----------"))

    TArray<FAGRDebugProp> DebugProps;
    GetDebugProps(DebugProps);

    UpdateCategoryTitleWithShortcut(SettingsCDO->CategoryCustom, AnimBPCustomPropsVerticalBox);
    UpdateCategoryTitleWithShortcut(SettingsCDO->CategoryAnimState, AGRAnimStatesVerticalBox);
    UpdateCategoryTitleWithShortcut(SettingsCDO->CategoryAimOffset, AGRAimOffsetVerticalBox);
    UpdateCategoryTitleWithShortcut(SettingsCDO->CategoryRotation, AGRRotationVerticalBox);
    UpdateCategoryTitleWithShortcut(SettingsCDO->CategoryRuntime, AGRRuntimeVerticalBox);
    UpdateCategoryTitleWithShortcut(SettingsCDO->CategoryMovement, AGRMovementVerticalBox);
    UpdateCategoryTitleWithShortcut(SettingsCDO->CategoryState, AGRStateVerticalBox);
    UpdateCategoryTitleWithShortcut(SettingsCDO->CategoryLeans, AGRLeansVerticalBox);
    UpdateCategoryTitleWithShortcut(SettingsCDO->CategorySetup, AGRSetupVerticalBox);

    for(FAGRDebugProp DebugProp : DebugProps)
    {
        const FProperty* Prop = DebugProp.Property;
        FString CPPType = Prop->GetCPPType();
        const TMap<FName, FString>* MetaDataMap = Prop->GetMetaDataMap();

        AGR_LOG(Log, TEXT("    Prop %s (%s) - Type: %s"), *Prop->GetName(), *DebugProp.Category, *CPPType);
        AGR_LOG(Log, TEXT("    Prop Class: %s"), *Prop->GetOwnerClass()->GetName());
        if(MetaDataMap != nullptr)
        {
            for(const TPair<FName, FString>& MetaData : *MetaDataMap)
            {
                AGR_LOG(Log, TEXT("    Metadata %s: %s"), *MetaData.Key.ToString(), *MetaData.Value);
            }
        }

        // Custom child Anim BP props
        if(Prop->GetOwnerClass() != UAGRCoreAnimInstance::StaticClass())
        {
            SetupVerticalBoxContainer(DebugProp, AnimBPCustomPropsVerticalBox, CustomProps);
        }

        // Anim States
        if(DebugProp.Category == "AGR|Anim States")
        {
            SetupVerticalBoxContainer(DebugProp, AGRAnimStatesVerticalBox, AnimStateProps);
        }

        // AimOffset
        if(DebugProp.Category == "AGR|AimOffset")
        {
            SetupVerticalBoxContainer(DebugProp, AGRAimOffsetVerticalBox, AimOffsetProps);
        }

        // Rotation
        if(DebugProp.Category == "AGR|Rotation")
        {
            SetupVerticalBoxContainer(DebugProp, AGRRotationVerticalBox, RotationProps);
        }

        // Runtime
        if(DebugProp.Category == "AGR|Runtime")
        {
            SetupVerticalBoxContainer(DebugProp, AGRRuntimeVerticalBox, RuntimeProps);
        }

        // Movement
        if(DebugProp.Category == "AGR|Movement")
        {
            SetupVerticalBoxContainer(DebugProp, AGRMovementVerticalBox, MovementProps);
        }

        // State
        if(DebugProp.Category == "AGR|State")
        {
            SetupVerticalBoxContainer(DebugProp, AGRStateVerticalBox, StateProps);
        }

        // Leans
        if(DebugProp.Category == "AGR|Leans")
        {
            SetupVerticalBoxContainer(DebugProp, AGRLeansVerticalBox, LeansProps);
        }

        // Setup
        if(DebugProp.Category == "AGR|Setup")
        {
            SetupVerticalBoxContainer(DebugProp, AGRSetupVerticalBox, SetupProps);
        }
    }

    if(CustomProps.Num() == 0 && AGRDebugPanelCustomProps)
    {
        AGRDebugPanelCustomProps->SetVisibility(ESlateVisibility::Collapsed);
    }

    bPropsInitialized = true;
}

void UAGRUWDebugWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if(!OwnerAnimInstance.IsValid())
    {
        return;
    }

    if(CustomProps.Num() != 0)
    {
        UpdatePropValues(CustomProps);
    }

    UpdatePropValues(AnimStateProps);
    UpdatePropValues(AimOffsetProps);
    UpdatePropValues(RotationProps);
    UpdatePropValues(RuntimeProps);
    UpdatePropValues(MovementProps);
    UpdatePropValues(StateProps);
    UpdatePropValues(LeansProps);
    UpdatePropValues(SetupProps);
}


void UAGRUWDebugWidget::GetDebugProps(TArray<FAGRDebugProp>& DebugProps)
{
    for(TFieldIterator<FProperty> PropIt(OwnerAnimInstance->GetClass()); PropIt; ++PropIt)
    {
        FProperty* Prop = *PropIt;

        const UClass* OwnerClass = Prop->GetOwnerClass();
        if(!OwnerClass->IsChildOf(UAGRCoreAnimInstance::StaticClass()))
        {
            break;
        }

        const FString Category = Prop->GetMetaData("Category");
        if(!Category.IsEmpty())
        {
            DebugProps.Add(FAGRDebugProp(Category, Prop));
        }
    }
}

UHorizontalBox* UAGRUWDebugWidget::CreateHorizontalBox(const FAGRDebugProp DebugProp)
{
    const FProperty* Property = DebugProp.Property;
    if(!Property)
    {
        return nullptr;
    }

    UHorizontalBox* HorizontalBox = NewObject<UHorizontalBox>();

    UTextBlock* TextBlockName = NewObject<UTextBlock>();
    TextBlockName->SetText(FText::FromString(Property->GetName()));
    TextBlockName->SetMinDesiredWidth(TextBlockNameWidth);
    TextBlockName->SetJustification(ETextJustify::Right);
    FString FontName = TextBlockName->GetDefaultFontName();
    TextBlockName->SetFont(FontInfo);

    UHorizontalBoxSlot* BoxSlot = HorizontalBox->AddChildToHorizontalBox(TextBlockName);
    BoxSlot->SetPadding(FMargin(8.f, 8.f));
    BoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);

    UImage* Image = NewObject<UImage>();
    Image->SetBrushTintColor(WhiteColor);
    Image->SetDesiredSizeOverride(FVector2D(2.f, 0.f));
    HorizontalBox->AddChildToHorizontalBox(Image);

    UTextBlock* TextBlockValue = NewObject<UTextBlock>();
    TextBlockValue->SetText(FText::FromString("---"));
    TextBlockValue->SetFont(FontInfo);
    TextBlockValue->SetMinDesiredWidth(TextBlockValueWidth);
    TextBlockValue->SetJustification(ETextJustify::Center);

    UHorizontalBoxSlot* BoxSlotValue = HorizontalBox->AddChildToHorizontalBox(TextBlockValue);
    BoxSlotValue->SetSize(ESlateSizeRule::Fill);
    BoxSlotValue->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);

    ValuesMap.Add(Property->GetName(), TextBlockValue);

    return HorizontalBox;
}

void UAGRUWDebugWidget::UpdatePropValues(TArray<FAGRDebugProp> DebugProps)
{
    for(FAGRDebugProp DebugProp : DebugProps)
    {
        FProperty* Property = DebugProp.Property;
        if(!Property)
        {
            continue;
        }

        UTextBlock* TextBlockValue = *ValuesMap.Find(Property->GetName());
        if(!IsValid(TextBlockValue))
        {
            return;
        }

        FProperty* AnimProperty = AnimClass->FindPropertyByName(FName(*Property->GetName()));
        FString CPPType = AnimProperty->GetCPPType();

        if(CPPType == "FGameplayTag")
        {
            FGameplayTag* PropPtr = AnimProperty->ContainerPtrToValuePtr<FGameplayTag>(OwnerAnimInstance.Get());
            if(PropPtr)
            {
                const FGameplayTag PropValue = *PropPtr;
                TextBlockValue->SetText(FText::FromString(PropValue.ToString()));
            }
        }
        else if(CPPType == "FGameplayTagContainer")
        {
            FGameplayTagContainer* PropPtr = AnimProperty->ContainerPtrToValuePtr<FGameplayTagContainer>(
                OwnerAnimInstance.Get());
            if(PropPtr)
            {
                const FGameplayTagContainer PropValue = *PropPtr;
                TextBlockValue->SetText(FText::FromString(PropValue.ToString()));
            }
        }
        else if(CPPType == "FRotator")
        {
            FRotator* PropPtr = AnimProperty->ContainerPtrToValuePtr<FRotator>(OwnerAnimInstance.Get());
            if(PropPtr)
            {
                const FRotator PropValue = *PropPtr;
                TextBlockValue->SetText(FText::FromString(PropValue.ToString()));
            }
        }
        else if(CPPType == "EAimOffsets")
        {
            EAGR_AimOffsets* PropPtr = AnimProperty->ContainerPtrToValuePtr<EAGR_AimOffsets>(OwnerAnimInstance.Get());
            if(PropPtr)
            {
                const EAGR_AimOffsets PropValue = *PropPtr;
                const FString AimOffsetString = PropValue == EAGR_AimOffsets::Aim
                                                ? TEXT("Aim")
                                                : PropValue == EAGR_AimOffsets::Look
                                                ? TEXT("Look")
                                                : TEXT("NONE");

                TextBlockValue->SetText(FText::FromString(AimOffsetString));
            }
        }
        else if(CPPType == "EAimOffsetClamp")
        {
            EAGR_AimOffsetClamp* PropPtr = AnimProperty->ContainerPtrToValuePtr<EAGR_AimOffsetClamp>(
                OwnerAnimInstance.Get());
            if(PropPtr)
            {
                const EAGR_AimOffsetClamp PropValue = *PropPtr;
                const FString AimOffsetString = PropValue == EAGR_AimOffsetClamp::Left
                                                ? TEXT("Left")
                                                : PropValue == EAGR_AimOffsetClamp::Nearest
                                                ? TEXT("Nearest")
                                                : PropValue == EAGR_AimOffsetClamp::Right
                                                ? TEXT("Right")
                                                : TEXT("NONE");

                TextBlockValue->SetText(FText::FromString(AimOffsetString));
            }
        }
        else if(CPPType == "ERotationMethod")
        {
            EAGR_RotationMethod* PropPtr = AnimProperty->ContainerPtrToValuePtr<EAGR_RotationMethod>(
                OwnerAnimInstance.Get());
            if(PropPtr)
            {
                const EAGR_RotationMethod PropValue = *PropPtr;
                const FString AimOffsetString = PropValue == EAGR_RotationMethod::AbsoluteRotation
                                                ? TEXT("AbsoluteRotation")
                                                : PropValue == EAGR_RotationMethod::DesiredRotation
                                                ? TEXT("DesiredRotation")
                                                : PropValue == EAGR_RotationMethod::DesiredAtAngle
                                                ? TEXT("DesiredAtAngle")
                                                : PropValue == EAGR_RotationMethod::RotateToVelocity
                                                ? TEXT("RotateToVelocity")
                                                : PropValue == EAGR_RotationMethod::NONE
                                                ? TEXT("NONE")
                                                : TEXT("NONE");

                TextBlockValue->SetText(FText::FromString(AimOffsetString));
            }
        }
        else if(CPPType == "TEnumAsByte<EMovementMode>")
        {
            TEnumAsByte<EMovementMode>* PropPtr = AnimProperty->ContainerPtrToValuePtr<TEnumAsByte<EMovementMode>>(
                OwnerAnimInstance.Get());
            if(PropPtr)
            {
                const TEnumAsByte<EMovementMode> PropValue = *PropPtr;
                const FString AimOffsetString = PropValue == EMovementMode::MOVE_Custom
                                                ? TEXT("Custom")
                                                : PropValue == EMovementMode::MOVE_Falling
                                                ? TEXT("Falling")
                                                : PropValue == EMovementMode::MOVE_Flying
                                                ? TEXT("Flying")
                                                : PropValue == EMovementMode::MOVE_None
                                                ? TEXT("None")
                                                : PropValue == EMovementMode::MOVE_Swimming
                                                ? TEXT("Swimming")
                                                : PropValue == EMovementMode::MOVE_Walking
                                                ? TEXT("Walking")
                                                : PropValue == EMovementMode::MOVE_NavWalking
                                                ? TEXT("Navmesh Walking")
                                                : PropValue == EMovementMode::MOVE_MAX
                                                ? TEXT("MOVE_MAX")
                                                : TEXT("NONE");

                TextBlockValue->SetText(FText::FromString(AimOffsetString));
            }
        }
        else if(CPPType == "FVector")
        {
            FVector* PropPtr = AnimProperty->ContainerPtrToValuePtr<FVector>(OwnerAnimInstance.Get());
            if(PropPtr)
            {
                const FVector PropValue = *PropPtr;
                TextBlockValue->SetText(FText::FromString(PropValue.ToString()));
            }
        }
        else if(CPPType == "FVector2D")
        {
            FVector2D* PropPtr = AnimProperty->ContainerPtrToValuePtr<FVector2D>(OwnerAnimInstance.Get());
            if(PropPtr)
            {
                const FVector2D PropValue = *PropPtr;
                TextBlockValue->SetText(FText::FromString(PropValue.ToString()));
            }
        }
        else if(CPPType == "float")
        {
            float* PropPtr = AnimProperty->ContainerPtrToValuePtr<float>(OwnerAnimInstance.Get());
            if(PropPtr)
            {
                const float PropValue = *PropPtr;
                TextBlockValue->SetText(FText::FromString(FString::SanitizeFloat(PropValue)));
            }
        }
        else if(CPPType == "bool")
        {
            bool* PropPtr = AnimProperty->ContainerPtrToValuePtr<bool>(OwnerAnimInstance.Get());
            if(PropPtr)
            {
                const bool PropValue = *PropPtr;
                TextBlockValue->SetText(FText::FromString(PropValue ? TEXT("true") : TEXT("false")));
                TextBlockValue->SetColorAndOpacity(FSlateColor(PropValue ? GreenColor : RedColor));
            }
        }
        else if(CPPType == "bool")
        {
            bool* PropPtr = AnimProperty->ContainerPtrToValuePtr<bool>(OwnerAnimInstance.Get());
            if(PropPtr)
            {
                const bool PropValue = *PropPtr;
                TextBlockValue->SetText(FText::FromString(PropValue ? TEXT("true") : TEXT("false")));
                TextBlockValue->SetColorAndOpacity(FSlateColor(PropValue ? GreenColor : RedColor));
            }
        }
        else
        {
            FString Str = FString::Format(TEXT("Not Supported - {0}"), {CPPType});
            TextBlockValue->SetText(FText::FromString(Str));
        }
    }
}

void UAGRUWDebugWidget::UpdateCategoryTitleWithShortcut(const FAGRDebuggerSettingsKey SettingsKey, UVerticalBox* Panel)
{
    const FKey InputKey = SettingsKey.InputKey;
    if(!IsValid(Panel) || !InputKey.IsValid())
    {
        return;
    }

    UTextBlock* TitleTextBlock = Cast<UTextBlock>(Panel->GetChildAt(1));
    if(IsValid(TitleTextBlock))
    {
        const FText TitleText = TitleTextBlock->GetText();
        const FString FormattedString = FString::Format(
            TEXT("{0} ({1})"),
            {TitleText.ToString(), SettingsKey.GetInputText().ToString()}
        );

        TitleTextBlock->SetText(FText::FromString(FormattedString));
    }
}

void UAGRUWDebugWidget::SetupVerticalBoxContainer(
    const FAGRDebugProp DebugProp,
    UVerticalBox* TargetVerticalBox,
    TArray<FAGRDebugProp>& TargetProps
)
{
    if(!IsValid(TargetVerticalBox))
    {
        return;
    }

    UHorizontalBox* Box = CreateHorizontalBox(DebugProp);
    UVerticalBox* VBox = NewObject<UVerticalBox>();
    VBox->AddChildToVerticalBox(Box);
    TargetVerticalBox->AddChildToVerticalBox(VBox);

    PropsContainers.Add(FAGRPropContainer(DebugProp.Category, VBox));
    TargetProps.Add(DebugProp);
}

#endif