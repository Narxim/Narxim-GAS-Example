// Copyright 2021 Joseph "Narxim" Thigpen.


#include "EffectWidgetControllerBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "EffectWidgetBase.h"
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
		RemoveEffect();
		return ETriggerUpdateStatus::NotValid;
	}

	UpdateEffectFromHandle();

	if (!EffectData || EffectData->IsPendingRemove)
	{
		RemoveEffect();
		return ETriggerUpdateStatus::NotValid;
	}

	UpdateFields();
	
	return K2_TriggerUpdate();
}

void UEffectWidgetControllerBase::InitializeController_Implementation(APlayerController* InOwningPlayer, UPanelWidget* InPanelWidget, FActiveGameplayEffectHandle Handle, const UCustomGameplayEffectUIData* const InEffectUIData)
{
	if (!InPanelWidget || !InOwningPlayer || !Handle.IsValid() || !InEffectUIData)
	{
		return;
	}
	
	PanelWidget = InPanelWidget;
	OwningPlayer = InOwningPlayer;
	EffectUIData = InEffectUIData;
	ActiveEffectHandle = Handle;

	UpdateEffectFromHandle();

	if (!EffectData || EffectData->IsPendingRemove)
	{
		RemoveEffect();
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

	// Subscribe to all available events on the gameplay effect event set, so that the controller can respond accordingly
	FActiveGameplayEffectEvents* const EventSet = ASC->GetActiveEffectEventSet(Handle);
	EventSet->OnEffectRemoved.AddUObject(this, &ThisClass::OnEffectRemoved);
	EventSet->OnStackChanged.AddUObject(this, &ThisClass::OnStackChanged);
	EventSet->OnInhibitionChanged.AddUObject(this, &ThisClass::OnInhibitionChanged);
	EventSet->OnTimeChanged.AddUObject(this, &ThisClass::OnTimeChanged);

	UpdateFields();
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
	return EffectData ? EffectData->GetDuration() : -1.f;
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
	
	OnTimeChanged(ActiveEffectHandle, EffectData->StartServerWorldTime, EffectData->GetDuration());
	OnStackChanged(ActiveEffectHandle, EffectData->ClientCachedStackCount, 0);
	
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
	K0_OnEffectRemoved(GameplayEffectRemovalInfo);
	RemoveEffect();
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

void UEffectWidgetControllerBase::K0_OnEffectRemoved_Implementation(const FGameplayEffectRemovalInfo& GameplayEffectRemovalInfo)
{
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
}

void UEffectWidgetControllerBase::K0_OnTimeChanged_Implementation(const float NewStartTime, const float NewDuration)
{
	if (!Widget)
	{
		return;
	}
	Widget->UpdateDuration(TotalDuration);
}

void UEffectWidgetControllerBase::RemoveEffect()
{
	if (Widget)
	{
		Widget->RemoveFromParent();
		Widget = nullptr;
	}
	K2_RemoveEffect();
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
