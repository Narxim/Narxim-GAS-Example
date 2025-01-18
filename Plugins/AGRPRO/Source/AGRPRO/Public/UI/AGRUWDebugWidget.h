// Copyright Adam Grodzki All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/UnrealType.h"
#include "Blueprint/UserWidget.h"

#include "AGRUWDebugWidget.generated.h"

struct FAGRDebuggerSettingsKey;
class UAGRAnimMasterComponent;
class UAGRCoreAnimInstance;
class UVerticalBox;
class UHorizontalBox;
class UTextBlock;
class UCanvasPanel;

USTRUCT()
struct FAGRDebugProp
{
    GENERATED_BODY()

    FString Category;

    FProperty* Property;

    FAGRDebugProp(const FString& Category, FProperty* PropertyField)
        : Category(Category),
          Property(PropertyField)
    {
    }

    FAGRDebugProp()
        : Property(nullptr)
    {
    }
};

USTRUCT()
struct FAGRPropContainer
{
    GENERATED_BODY()

    FString Category;

    UPROPERTY()
    UVerticalBox* Container;

    FAGRPropContainer(const FString& Category, UVerticalBox* Container)
        : Category(Category),
          Container(Container)
    {
    }

    FAGRPropContainer()
        : Container(nullptr)
    {
    }
};

UCLASS()
class AGRPRO_API UAGRUWDebugWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
#if WITH_EDITORONLY_DATA
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "AGR|UI")
    UVerticalBox* AGRVariablesVerticalBox = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "AGR|UI")
    UVerticalBox* AGRAnimStatesVerticalBox = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "AGR|UI")
    UVerticalBox* AGRAimOffsetVerticalBox = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "AGR|UI")
    UVerticalBox* AGRRotationVerticalBox = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "AGR|UI")
    UVerticalBox* AGRRuntimeVerticalBox = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "AGR|UI")
    UVerticalBox* AGRMovementVerticalBox = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "AGR|UI")
    UVerticalBox* AGRStateVerticalBox = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "AGR|UI")
    UVerticalBox* AGRLeansVerticalBox = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "AGR|UI")
    UVerticalBox* AGRSetupVerticalBox = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "AGR|UI")
    UVerticalBox* AnimBPCustomPropsVerticalBox = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "AGR|UI")
    UCanvasPanel* AGRDebugPanelCustomProps = nullptr;
#endif

#if WITH_EDITOR
    FSlateFontInfo FontInfo = FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 12);

    /** Desired with for property names TextBlock (can think of exposing that to UMG Blueprint) */
    float TextBlockNameWidth = 200.f;

    /** Desired with for property names TextBlock (can think of exposing that to UMG Blueprint) */
    float TextBlockValueWidth = 350.f;

    /** Holds the UClass for anim instance to lookup for properties */
    UClass* AnimClass;

    /** Maps of Property Name to associated UTextBlock to update values on tick*/
    TMap<FString, UTextBlock*> ValuesMap;

    // ~ Lists of DebugProp associated to a given category
    TArray<FAGRDebugProp> CustomProps;
    TArray<FAGRDebugProp> AnimStateProps;
    TArray<FAGRDebugProp> AimOffsetProps;
    TArray<FAGRDebugProp> RotationProps;
    TArray<FAGRDebugProp> RuntimeProps;
    TArray<FAGRDebugProp> MovementProps;
    TArray<FAGRDebugProp> StateProps;
    TArray<FAGRDebugProp> LeansProps;
    TArray<FAGRDebugProp> SetupProps;

    // List of VerticalBox container for each props categories, to be able to toggle them on / off
    TArray<FAGRPropContainer> PropsContainers;

    // ~ TWeakObject pointers to reference we need to keep reference to
    TWeakObjectPtr<ACharacter> OwnerCharacter;
    TWeakObjectPtr<APlayerController> OwnerPlayerController;
    TWeakObjectPtr<UAGRAnimMasterComponent> OwnerAnimMaster;
    TWeakObjectPtr<UAGRCoreAnimInstance> OwnerAnimInstance;

    // ~ Colors used to display booleans
    FLinearColor WhiteColor = FLinearColor(1.f, 1.f, 1.f, 1.f);
    FLinearColor GreenColor = FLinearColor(0.729412f, 0.854902f, 0.333333f, 1.f);
    FLinearColor RedColor = FLinearColor(1.f, 0.388235f, 0.278431f, 1.f);

    // Failsafe check to only construct once
    bool bPropsInitialized = false;
#endif

public:
#if WITH_EDITOR
    void SetupOwner(APlayerController* OwningPlayer);
    void ToggleCategory(FString Category);
#endif

protected:
#if WITH_EDITOR
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // Iterates through each Properties of the anim class and struct with category info
    virtual void GetDebugProps(OUT TArray<FAGRDebugProp>& DebugProps);

    // Creates the horizontal box for a given property
    virtual UHorizontalBox* CreateHorizontalBox(FAGRDebugProp DebugProp);

    // Updates actual property values on tick
    virtual void UpdatePropValues(TArray<FAGRDebugProp> DebugProps);

    // Edits the category panel title with configured shortcut (if any)
    virtual void UpdateCategoryTitleWithShortcut(FAGRDebuggerSettingsKey SettingsKey, UVerticalBox* Panel);

    // Creates and setup horizontal / vertical boxes for categories panel
    void SetupVerticalBoxContainer(
        FAGRDebugProp DebugProp,
        UVerticalBox* TargetVerticalBox,
        TArray<FAGRDebugProp>& TargetProps);
#endif
};