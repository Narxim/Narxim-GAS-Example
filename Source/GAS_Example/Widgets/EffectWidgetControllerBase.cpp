// Copyright 2021 Joseph "Narxim" Thigpen.


#include "EffectWidgetControllerBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "EffectWidgetBase.h"
#include "GAS_Example/AbilitySystem/AbilitySystemComponent/CustomAbilitySystemComponent.h"
#include "GAS_Example/AbilitySystem/AbilitySystemComponent/CustomGameplayEffectUIData.h"

class UWorld* UEffectWidgetControllerBase::GetWorld() const
{
	if (GetOuter())
	{
		return GetOuter()->GetWorld();
	}

	return nullptr;
}

ETriggerUpdateStatus UEffectWidgetControllerBase::TriggerUpdate()
{
	if (!OwningPlayer || !Widget)
	{
		return ETriggerUpdateStatus::NotValid;
	}

	if (!ActiveEffectHandle.IsValid())
	{
		RemoveEffectAndController();
		return ETriggerUpdateStatus::NotValid;
	}

	UpdateEffectFromHandle();

	if (!EffectData || EffectData->IsPendingRemove)
	{
		RemoveEffectAndController();
		return ETriggerUpdateStatus::NotValid;
	}

	UpdateFields();
	
	return K2_TriggerUpdate();
}

const UCustomAbilitySystemComponent* UEffectWidgetControllerBase::GetAbilitySystem() const
{
	return Cast<UCustomAbilitySystemComponent>(ActiveEffectHandle.GetOwningAbilitySystemComponent());
}

const FGameplayEffectSpec UEffectWidgetControllerBase::GetEffectSpec() const
{
	if (!EffectData)
	{
		return FGameplayEffectSpec();
	}
	return EffectData->Spec;
}

const UCustomGameplayEffectUIData* UEffectWidgetControllerBase::GetEffectUIData() const
{
	return EffectUIData.Get();
}

float UEffectWidgetControllerBase::GetSetByCallerMagnitude(const FGameplayTag DataTag, const bool bWarnIfNotFound, const float bDefaultIfNotFound) const
{
	return GetEffectSpec().GetSetByCallerMagnitude(DataTag, bWarnIfNotFound, bDefaultIfNotFound);
}

void UEffectWidgetControllerBase::SetActiveHandle(FActiveGameplayEffectHandle NewHandle)
{
	if (!NewHandle.IsValid())
	{
		return;
	}

	ActiveEffectHandle = NewHandle;
	if (!ActiveEffectHandle.IsValid())
	{
		RemoveEffectAndController();
		return;
	}
	
	TotalDuration = -1.f;
	CurrentStacks = -1.f;

	RegisterEvents(ActiveEffectHandle.GetOwningAbilitySystemComponent());
	TriggerUpdate();
}

void UEffectWidgetControllerBase::InitializeController_Implementation(APlayerController* InOwningPlayer, UPanelWidget* InPanelWidget, FActiveGameplayEffectHandle Handle, const UCustomGameplayEffectUIData* const InEffectUIData)
{
	if (!InPanelWidget || !InOwningPlayer || !Handle.IsValid() || !InEffectUIData)
	{
		return;
	}

	if (Widget)
	{
		Widget->SetController(this);
	}
	
	PanelWidget = InPanelWidget;
	OwningPlayer = InOwningPlayer;
	EffectUIData = InEffectUIData;
	ActiveEffectHandle = Handle;

	UpdateEffectFromHandle();

	if (!EffectData || EffectData->IsPendingRemove)
	{
		RemoveEffectAndController();
		return;
	}

	if (!Handle.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* const ASC = Handle.GetOwningAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}
	RegisterEvents(ASC);
	UpdateFields();
}

void UEffectWidgetControllerBase::RegisterEvents(UAbilitySystemComponent* const ASC)
{
	if (!ASC)
	{
		return;
	}
	
	// Subscribe to all available events on the gameplay effect event set, so that the controller can respond accordingly
    FActiveGameplayEffectEvents* const EventSet = ASC->GetActiveEffectEventSet(ActiveEffectHandle);
    EventSet->OnEffectRemoved.AddUObject(this, &ThisClass::OnEffectRemoved);
    EventSet->OnStackChanged.AddUObject(this, &ThisClass::OnStackChanged);
    EventSet->OnInhibitionChanged.AddUObject(this, &ThisClass::OnInhibitionChanged);
    EventSet->OnTimeChanged.AddUObject(this, &ThisClass::OnTimeChanged);
}

TSoftObjectPtr<UTexture2D> UEffectWidgetControllerBase::ExtractIcon_Implementation()
{
	return EffectUIData ? EffectUIData->IconTexture : nullptr;
}

FText UEffectWidgetControllerBase::ExtractDescription_Implementation()
{
	return EffectUIData->Description;
}

FText UEffectWidgetControllerBase::ExtractTitle_Implementation()
{
	return EffectUIData->Title;
}

float UEffectWidgetControllerBase::CalculateDuration_Implementation(float NewDuration)
{
	if (NewDuration > -1.f)
	{
		return NewDuration;
	}
	return EffectData ? EffectData->GetTimeRemaining(GetWorld()->TimeSeconds) : -1.f;
}

int32 UEffectWidgetControllerBase::CalculateStackCount_Implementation(int32 NewStackCount)
{
	if (NewStackCount > -1)
	{
		return NewStackCount;
	}
	return EffectData ? EffectData->ClientCachedStackCount : 0;
}

void UEffectWidgetControllerBase::StartDurationUpdate()
{
	if (!Widget)
	{
		return;
	}
	
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (DurationUpdateTimer.IsValid())
	{
		TimerManager.ClearTimer(DurationUpdateTimer);
	}

	TimerManager.SetTimer(DurationUpdateTimer, this, &ThisClass::UpdateDurationOnWidget, DurationUpdateFrequency, true);
}

void UEffectWidgetControllerBase::UpdateDurationOnWidget()
{
	if (!Widget)
	{
		GetWorld()->GetTimerManager().ClearTimer(DurationUpdateTimer);
		return;
	}
	
	UpdateEffectFromHandle();
	const float RemainingDuration = EffectData->GetTimeRemaining(GetWorld()->GetTimeSeconds());

	Widget->UpdateDuration(FMath::Max(RemainingDuration, 0.f));
	
	if (RemainingDuration <= 0.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(DurationUpdateTimer);
	}
}

void UEffectWidgetControllerBase::UpdateFields()
{
	if (!EffectData)
	{
		return;
	}
	
	OnTimeChanged(ActiveEffectHandle, EffectData->StartServerWorldTime, EffectData->GetTimeRemaining(GetWorld()->TimeSeconds));
	OnStackChanged(ActiveEffectHandle, EffectData->ClientCachedStackCount, 0);
	OnInhibitionChanged(ActiveEffectHandle, EffectData->bIsInhibited);
	
	if (UE_MVVM_SET_PROPERTY_VALUE(Title, ExtractTitle()))
	{
		if (Widget)
		{
			Widget->UpdateTitle(Title);
		}
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Title);
	}
	
	if (UE_MVVM_SET_PROPERTY_VALUE(Description, ExtractDescription()))
	{
		if (Widget)
		{
			Widget->UpdateDescription(Description);
		}
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Description);
	}
	
	if (UE_MVVM_SET_PROPERTY_VALUE(SoftIcon, ExtractIcon()) || UE_MVVM_SET_PROPERTY_VALUE(SoftMaterial, ExtractSoftMaterial()))
	{
		if (Widget)
		{
			Widget->UpdateIcon(SoftIcon, SoftMaterial);
		}
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(SoftIcon);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(SoftMaterial);
	}
}

void UEffectWidgetControllerBase::UpdateEffectFromHandle()
{
	const UAbilitySystemComponent* ASC = ActiveEffectHandle.GetOwningAbilitySystemComponent();

	if (!ASC)
	{
		return;
	}
	
	EffectData = ASC->GetActiveGameplayEffect(ActiveEffectHandle);
}

void UEffectWidgetControllerBase::OnEffectRemoved(const FGameplayEffectRemovalInfo& GameplayEffectRemovalInfo)
{
	FGameplayEffectSpec Spec{};
	UClass* Class = nullptr;

	if (GameplayEffectRemovalInfo.ActiveEffect)
	{
		Spec = GameplayEffectRemovalInfo.ActiveEffect->Spec;
		Class = GameplayEffectRemovalInfo.ActiveEffect->Spec.Def.GetClass();
	}
	
	if (!K0_OnEffectRemoved(GameplayEffectRemovalInfo, Spec, Class))
	{
		return;
	}
	
	RemoveEffectAndController();
}

void UEffectWidgetControllerBase::OnStackChanged(FActiveGameplayEffectHandle ActiveGameplayEffectHandle, const int32 NewStackCount, const int32 OldStackCount)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(CurrentStacks, CalculateStackCount(NewStackCount)))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CurrentStacks);
		K0_OnStackChanged(NewStackCount, OldStackCount);
	}
}

void UEffectWidgetControllerBase::OnInhibitionChanged(FActiveGameplayEffectHandle ActiveGameplayEffectHandle, const bool bIsInhibited)
{
	K0_OnInhibitionChanged(bIsInhibited);
}

void UEffectWidgetControllerBase::OnTimeChanged(FActiveGameplayEffectHandle ActiveGameplayEffectHandle,	const float NewStartTime, const float NewDuration)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(TotalDuration, CalculateDuration(NewDuration)))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(TotalDuration);
		K0_OnTimeChanged(NewStartTime, TotalDuration);

		if (TotalDuration > 1.f)
		{
			StartDurationUpdate();
		}
	}
}

bool UEffectWidgetControllerBase::K0_OnEffectRemoved_Implementation(const FGameplayEffectRemovalInfo& GameplayEffectRemovalInfo, const FGameplayEffectSpec Spec, const TSubclassOf<
	                                                                    UGameplayEffect> EffectClass)
{
	return true;
}

void UEffectWidgetControllerBase::K0_OnStackChanged_Implementation(const int32 NewStackCount, const int32 OldStackCount)
{
	if (!Widget)
	{
		return;
	}
	Widget->UpdateStackCount(CurrentStacks);
}

void UEffectWidgetControllerBase::K0_OnInhibitionChanged_Implementation(const bool bIsInhibited)
{
	if (!Widget)
	{
		return;
	}
	Widget->UpdateInhibition(bIsInhibited);
	
}

void UEffectWidgetControllerBase::K0_OnTimeChanged_Implementation(const float NewStartTime, const float NewDuration)
{
	if (!Widget)
	{
		return;
	}
	Widget->UpdateDuration(TotalDuration);
}

TSubclassOf<UGameplayEffect> UEffectWidgetControllerBase::GetEffectClass() const
{
	if (!EffectData)
	{
		return nullptr;
	}
	
	return EffectData->Spec.Def.GetClass();
}

const UObject* UEffectWidgetControllerBase::GetEffectKey_Implementation() const
{
	return GetEffectClass();
}

void UEffectWidgetControllerBase::RemoveEffectAndController()
{
	if (Widget)
	{
		Widget->RemoveFromParent();
		Widget = nullptr;
	}
	K2_RemoveEffect();

	OnControllerOblsolete.Broadcast(this);
	
	MarkAsGarbage();
}

TSoftObjectPtr<UMaterialInterface> UEffectWidgetControllerBase::ExtractSoftMaterial_Implementation()
{
	return EffectUIData ? EffectUIData->IconMaterial : nullptr;
}

ETriggerUpdateStatus UEffectWidgetControllerBase::K2_TriggerUpdate_Implementation()
{
	return ETriggerUpdateStatus::UpdateOK;
}
