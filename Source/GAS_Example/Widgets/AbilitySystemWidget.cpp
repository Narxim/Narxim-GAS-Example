// Copyright 2021 Joseph "Narxim" Thigpen.

#include "AbilitySystemWidget.h"
#include "AbilitySystemGlobals.h"
#include "GAS_Example/AbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "GAS_Example/AbilitySystem/AttributeSets/LevelAttributeSet.h"
#include "GAS_Example/AbilitySystem/AttributeSets/StaminaAttributeSet.h"


UAbilitySystemComponent* UAbilitySystemWidget::GetOwnerAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

bool UAbilitySystemWidget::InitializeAbilitySystemWidget(UAbilitySystemComponent* InOwnerAbilitySystemComponent)
{
	UAbilitySystemComponent* OldAbilitySystemComponent = AbilitySystemComponent.Get();
	
	AbilitySystemComponent = InOwnerAbilitySystemComponent;
	
	// The Ability System Component is invalid. Stop here and return false. 
	if (!GetOwnerAbilitySystemComponent())
	{
		return false;
	}

	// Reset any old Attribute Change Delegates if they are still bound.
	if (IsValid(OldAbilitySystemComponent))
	{
		ResetDelegateHandle(MaximumHealthChangeDelegate, OldAbilitySystemComponent, UHealthAttributeSet::GetMaximumHealthAttribute());
		ResetDelegateHandle(CurrentHealthChangeDelegate, OldAbilitySystemComponent, UHealthAttributeSet::GetCurrentHealthAttribute());
		ResetDelegateHandle(HealthRegenerationChangeDelegate, OldAbilitySystemComponent, UHealthAttributeSet::GetHealthRegenerationAttribute());
		ResetDelegateHandle(MaximumStaminaChangeDelegate, OldAbilitySystemComponent, UStaminaAttributeSet::GetMaximumStaminaAttribute());
		ResetDelegateHandle(CurrentStaminaChangeDelegate, OldAbilitySystemComponent, UStaminaAttributeSet::GetCurrentStaminaAttribute());
		ResetDelegateHandle(StaminaRegenerationChangeDelegate, OldAbilitySystemComponent, UStaminaAttributeSet::GetStaminaRegenerationAttribute());
		ResetDelegateHandle(LevelChangeDelegate, OldAbilitySystemComponent, ULevelAttributeSet::GetCurrentLevelAttribute());
	}

	bool bBindingDone = false;
	
	// Bind Health attribute delegates if the Ability System Component has the required Attribute Set -and- we are listening for Health attributes.
	if (ListenForHealthAttributeSetChanges && AbilitySystemComponent->HasAttributeSetForAttribute(UHealthAttributeSet::GetMaximumHealthAttribute()))
	{
		bBindingDone = true;
		
		MaximumHealthChangeDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetMaximumHealthAttribute()).AddUObject(this, &UAbilitySystemWidget::MaximumHealthChanged);
		CurrentHealthChangeDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetCurrentHealthAttribute()).AddUObject(this, &UAbilitySystemWidget::CurrentHealthChanged);
		HealthRegenerationChangeDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetHealthRegenerationAttribute()).AddUObject(this, &UAbilitySystemWidget::HealthRegenerationChanged);
		
		const float MaxHealth = AbilitySystemComponent->GetNumericAttribute(UHealthAttributeSet::GetMaximumHealthAttribute());
		const float CurrentHealth = AbilitySystemComponent->GetNumericAttribute(UHealthAttributeSet::GetCurrentHealthAttribute());
			
		// Call the Blueprint Events to initialize the values.
		On_MaximumHealthChanged(MaxHealth, 0.0f, CurrentHealth / MaxHealth);
		On_CurrentHealthChanged(CurrentHealth, 0.0f, CurrentHealth / MaxHealth);
		On_HealthRegenerationChanged(AbilitySystemComponent->GetNumericAttribute(UHealthAttributeSet::GetHealthRegenerationAttribute()), 0.0f);
	}
	
	// Bind Stamina attribute delegates if the Ability System Component has the required Attribute Set -and- we are listening for Stamina attributes.
	if (ListenForStaminaAttributeSetChanges && AbilitySystemComponent->HasAttributeSetForAttribute(UStaminaAttributeSet::GetMaximumStaminaAttribute()))
	{
		bBindingDone = true;
		
		MaximumStaminaChangeDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UStaminaAttributeSet::GetMaximumStaminaAttribute()).AddUObject(this, &UAbilitySystemWidget::MaximumStaminaChanged);
		CurrentStaminaChangeDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UStaminaAttributeSet::GetCurrentStaminaAttribute()).AddUObject(this, &UAbilitySystemWidget::CurrentStaminaChanged);
		StaminaRegenerationChangeDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UStaminaAttributeSet::GetStaminaRegenerationAttribute()).AddUObject(this, &UAbilitySystemWidget::StaminaRegenerationChanged);

		const float MaxStamina = AbilitySystemComponent->GetNumericAttribute(UStaminaAttributeSet::GetMaximumStaminaAttribute());
		const float CurrentStamina = AbilitySystemComponent->GetNumericAttribute(UStaminaAttributeSet::GetCurrentStaminaAttribute());
	
		// Call the Blueprint Events to initialize the values.
		On_MaximumStaminaChanged(MaxStamina, 0.0f, CurrentStamina / MaxStamina);
		On_CurrentStaminaChanged(CurrentStamina, 0.0f, CurrentStamina / MaxStamina);
		On_StaminaRegenerationChanged(AbilitySystemComponent->GetNumericAttribute(UStaminaAttributeSet::GetStaminaRegenerationAttribute()), 0.0f);
	}

	if (AbilitySystemComponent->HasAttributeSetForAttribute(ULevelAttributeSet::GetMaximumLevelAttribute()))
	{
		LevelChangeDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(ULevelAttributeSet::GetCurrentLevelAttribute()).AddUObject(this, &UAbilitySystemWidget::CurrentLevelChanged);
		const float CurrentLevel = AbilitySystemComponent->GetNumericAttribute(ULevelAttributeSet::GetCurrentLevelAttribute());
		On_LevelChanged(CurrentLevel, 0.f);
	}
	
	return bBindingDone;
}

void UAbilitySystemWidget::CurrentLevelChanged(const FOnAttributeChangeData& Data)
{
	On_LevelChanged(Data.NewValue, Data.OldValue);
}

void UAbilitySystemWidget::MaximumHealthChanged(const FOnAttributeChangeData& Data)
{
	const float CurrentHealth = AbilitySystemComponent->GetNumericAttribute(UHealthAttributeSet::GetCurrentHealthAttribute());

	On_MaximumHealthChanged(Data.NewValue, Data.OldValue, CurrentHealth / Data.NewValue);
}

void UAbilitySystemWidget::CurrentHealthChanged(const FOnAttributeChangeData& Data)
{
	const float MaxHealth = AbilitySystemComponent->GetNumericAttribute(UHealthAttributeSet::GetMaximumHealthAttribute());

	On_CurrentHealthChanged(Data.NewValue, Data.OldValue, Data.NewValue / MaxHealth);
}

void UAbilitySystemWidget::HealthRegenerationChanged(const FOnAttributeChangeData& Data)
{
	On_HealthRegenerationChanged(Data.NewValue, Data.OldValue);
}

void UAbilitySystemWidget::MaximumStaminaChanged(const FOnAttributeChangeData& Data)
{
	const float CurrentStamina = AbilitySystemComponent->GetNumericAttribute(UStaminaAttributeSet::GetCurrentStaminaAttribute());

	On_MaximumStaminaChanged(Data.NewValue, Data.OldValue, CurrentStamina / Data.NewValue);
}

void UAbilitySystemWidget::CurrentStaminaChanged(const FOnAttributeChangeData& Data)
{
	const float MaxStamina = AbilitySystemComponent->GetNumericAttribute(UStaminaAttributeSet::GetMaximumStaminaAttribute());

	On_CurrentStaminaChanged(Data.NewValue, Data.OldValue, Data.NewValue / MaxStamina);
}

void UAbilitySystemWidget::StaminaRegenerationChanged(const FOnAttributeChangeData& Data)
{
	On_StaminaRegenerationChanged(Data.NewValue, Data.OldValue);
}

void UAbilitySystemWidget::ResetDelegateHandle(FDelegateHandle DelegateHandle, UAbilitySystemComponent* OldAbilitySystemComponent, const FGameplayAttribute& Attribute)
{
	if (IsValid(OldAbilitySystemComponent))
	{
		OldAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).Remove(DelegateHandle);
		DelegateHandle.Reset();
	}
}