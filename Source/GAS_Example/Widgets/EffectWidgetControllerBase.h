// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "MVVMViewModelBase.h"
#include "MVVMViewModelBase.h"
#include "UObject/Object.h"
#include "EffectWidgetControllerBase.generated.h"

enum class ECustomExecPin_Valid2 : uint8;
class UPanelWidget;
class UCustomAbilitySystemComponent;
class UEffectWidgetBase;
class UCustomGameplayEffectUIData;

UENUM(BlueprintType)
enum class ETriggerUpdateStatus: uint8
{
	UpdateOK,
	NoUpdate,
	NotValid
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnControllerOblsoleteDelegate, UEffectWidgetControllerBase*, Controller);

/**
 * This class represents a Gameplay effect that have been applied to the player which REQUIRES a UI Visualization.
 * The goal is to react/handle all gameplay effect events (add, changed, removed) and ultimately CONTROL a GameplayEffect Widget (EffectWidgetBase).
 *
 * Most of the important methods can be overridden (Like Extract* and Calculate*), and the Widget class itself can be set on it, if you want your own.
 *
 * This object is constructed and used only on Client
 */
UCLASS(Abstract, Blueprintable, Config = Game)
class GAS_EXAMPLE_API UEffectWidgetControllerBase : public UMVVMViewModelBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DurationUpdateFrequency = 0.5f;
	
	// Panel where we should add the widget.
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UPanelWidget> PanelWidget;
	
	// Player Controller owning this effect / controller
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APlayerController> OwningPlayer;

	// The Handle to the effect.
	UPROPERTY(BlueprintReadOnly)
	FActiveGameplayEffectHandle ActiveEffectHandle;

	// Effect Data. Warning: CAN BE NULL
	const FActiveGameplayEffect* EffectData;

	// Associated UI data from the Gameplay effect. If no UI Data, no controller will be built.
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<const UCustomGameplayEffectUIData> EffectUIData;

	// Widget class to instantiate. Default is set in the editor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config)
	TSoftClassPtr<UEffectWidgetBase> WidgetClass;

	// Like to the spawned widget.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UEffectWidgetBase> Widget;

protected:
	// Total duration of the effect. Not updated except if server is changing it.
	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, meta=(AllowPrivateAccess))
	float TotalDuration;

	// Current Stacks. Updated everytime the stack changes. Will Call "CalculateStackCounts(NewStackCount)" and K0_OnStackChanged();
	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, meta=(AllowPrivateAccess))
	int32 CurrentStacks;

	// Effect Title, will call "ExtractTitle" if you ever want to change something. Called after ALL numeric parameters have been updated.
	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, meta=(AllowPrivateAccess))
	FText Title;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, meta=(AllowPrivateAccess))
	FText Description;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, meta=(AllowPrivateAccess))
	TSoftObjectPtr<UTexture2D> SoftIcon;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, meta=(AllowPrivateAccess))
	TSoftObjectPtr<UMaterialInterface> SoftMaterial;
	
	FTimerHandle DurationUpdateTimer;

public:
	UPROPERTY(BlueprintAssignable)
	FOnControllerOblsoleteDelegate OnControllerObsolete;

	UFUNCTION(BlueprintPure)
	TSubclassOf<UGameplayEffect> GetEffectClass() const;

	/*
	 * Return the effect key that will be used in the UI Widget bar.
	 * Return GetEffectClass by default
	 */ 
	UFUNCTION(BlueprintNativeEvent)
	const UObject* GetEffectKey() const;
	
	const float& GetTotalDuration() const
	{
		return TotalDuration;
	}

	const int32& GetCurrentStacks() const
	{
		return CurrentStacks;
	}

	const FText& GetTitle() const
	{
		return Title;
	}

	const FText& GetDescription() const
	{
		return Description;
	}

	const TSoftObjectPtr<UTexture2D>& GetSoftIcon() const
	{
		return SoftIcon;
	}

	const TSoftObjectPtr<UMaterialInterface>& GetSoftMaterial() const
	{
		return SoftMaterial;
	}

public:
	// Initialize the Controller with the required values.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool InitializeController(
		APlayerController* InOwningPlayer,
		UPanelWidget* InPanelWidget,
		FActiveGameplayEffectHandle Handle,
		const UCustomGameplayEffectUIData* InEffectUIData);

	// Trigger a full update
	UFUNCTION(BlueprintCallable)
	ETriggerUpdateStatus TriggerUpdate();

	UFUNCTION(BlueprintPure)
	const UCustomAbilitySystemComponent* GetAbilitySystem() const;

	UFUNCTION(BlueprintPure)
	const FGameplayEffectSpec GetEffectSpec() const;

	UFUNCTION(BlueprintPure)
	const UCustomGameplayEffectUIData* GetEffectUIData() const;

	UFUNCTION(BlueprintPure)
	float GetSetByCallerMagnitude(const FGameplayTag DataTag, const bool bWarnIfNotFound = true, const float bDefaultIfNotFound = 0.f) const;

protected:
	UFUNCTION(BlueprintCallable)
	void SetActiveHandle(FActiveGameplayEffectHandle NewHandle);
	
	void RegisterEvents(UAbilitySystemComponent* ASC);

	// Called after a full update.
	UFUNCTION(BlueprintNativeEvent)
	ETriggerUpdateStatus K2_TriggerUpdate();
	
	UFUNCTION(BlueprintCallable)
	void RemoveEffectAndController();

	UFUNCTION(BlueprintImplementableEvent)
	void K2_RemoveEffect();

	// Override GetWorld
	virtual bool ImplementsGetWorld() const override
	{
		return true;
	}
	
	virtual class UWorld* GetWorld() const override;

	// Extract the Icon we're supposed to use from the UI Effect Component. Can be overriden.
	UFUNCTION(BlueprintNativeEvent)
	TSoftObjectPtr<UTexture2D> ExtractIcon();
	
	// Extract the Material we're supposed to use from the UI Effect Component. Can be overriden.
	UFUNCTION(BlueprintNativeEvent)
	TSoftObjectPtr<UMaterialInterface> ExtractSoftMaterial();
	
	// Extract the Description we're supposed to use from the UI Effect Component. Can be overriden.
	UFUNCTION(BlueprintNativeEvent)
	FText ExtractDescription();

	// Extract the Title we're supposed to use from the UI Effect Component. Can be overriden.
	UFUNCTION(BlueprintNativeEvent)
	FText ExtractTitle();

	// Calculate the Stack count. Can be overriden.
	UFUNCTION(BlueprintNativeEvent)
	int32 CalculateStackCount(int32 NewStackCount = -1);

	// Calculate Duration. Can be overriden.
	UFUNCTION(BlueprintNativeEvent)
	float CalculateDuration(float NewDuration = -1.f);

	// Starts a timer to update the remaining time on the widget.
	void StartDurationUpdate();
	
	UFUNCTION()
	// Update the time on the widget.
	void UpdateDurationOnWidget();
	
	// Update all fields.
	UFUNCTION(BlueprintCallable)
	void UpdateFields(const bool bForceUpdate = false);
	
	// Extract the new active effect definition from handle.
	void UpdateEffectFromHandle();

	// Standard callback marking this controller as garbage and remove the Effect widget
	void OnEffectRemoved(const FGameplayEffectRemovalInfo& GameplayEffectRemovalInfo);
	
	// Standard callback updating CurrentStack on the controller, and calling the K0
	void OnStackChanged(FActiveGameplayEffectHandle ActiveGameplayEffectHandle, const int32 NewStackCount, const int32 OldStackCount);
	
	// Standard callback updating Inhibition flag on the controller, and calling the K0
	void OnInhibitionChanged(FActiveGameplayEffectHandle ActiveGameplayEffectHandle, const bool bIsInhibited);
	
	// Standard callback updating Total Duration on the controller, and calling the K0. Will Reset the timer.
	void OnTimeChanged(FActiveGameplayEffectHandle ActiveGameplayEffectHandle, const float NewStartTime, const float NewDuration);

	// Called right before full removal. Can be overriden.
	UFUNCTION(BlueprintNativeEvent)
	bool K0_OnEffectRemoved(const FGameplayEffectRemovalInfo& GameplayEffectRemovalInfo, FGameplayEffectSpec Spec, TSubclassOf<UGameplayEffect> EffectClass);
	
	// Updates the Stack on the Widget. Can be overriden.
	UFUNCTION(BlueprintNativeEvent)
	void K0_OnStackChanged(const int32 NewStackCount, const int32 OldStackCount);
	
	// Called when Inhibition changes. Can be overriden. (Doing nothing for now)
	UFUNCTION(BlueprintNativeEvent)
	void K0_OnInhibitionChanged(const bool bIsInhibited);
	
	// Updates the Duration on the Widget. Can be overriden.
	UFUNCTION(BlueprintNativeEvent)
	void K0_OnTimeChanged(const float NewStartTime, const float NewDuration);
};
