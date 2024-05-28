// Copyright 2021 Joseph "Narxim" Thigpen.

#include "AbilitySystemWidget.h"
#include "AbilitySystemGlobals.h"
#include "GAS_Example/AbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "GAS_Example/AbilitySystem/AttributeSets/StaminaAttributeSet.h"


UAbilitySystemComponent* UAbilitySystemWidget::GetOwnerAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

bool UAbilitySystemWidget::InitializeAbilitySystemWidget(UAbilitySystemComponent* InOwnerAbilitySystemComponent)
{
	AbilitySystemComponent = InOwnerAbilitySystemComponent;
	
	// The Ability System Component is invalid. Stop here and return false. 
	if (!GetOwnerAbilitySystemComponent())
	{
		return false;
	}
	
	// Reset the Attribute change Delegate Handles in case they are already bound.
	MaximumHealthChangeDelegate.Reset();
	CurrentHealthChangeDelegate.Reset();
	HealthRegenerationChangeDelegate.Reset();
	MaximumStaminaChangeDelegate.Reset();
	CurrentStaminaChangeDelegate.Reset();
	StaminaRegenerationChangeDelegate.Reset();
	
	// Bind Health attribute delegates if the Ability System Component has the required Attribute Set -and- we are listening for Health attributes.
	if (ListenForHealthAttributeSetChanges)
	{
		if (AbilitySystemComponent->HasAttributeSetForAttribute(UHealthAttributeSet::GetMaximumHealthAttribute()))
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetMaximumHealthAttribute()).AddUObject(this, &UAbilitySystemWidget::MaximumHealthChanged);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetCurrentHealthAttribute()).AddUObject(this, &UAbilitySystemWidget::CurrentHealthChanged);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetHealthRegenerationAttribute()).AddUObject(this, &UAbilitySystemWidget::HealthRegenerationChanged);

			const float MaxHealth = AbilitySystemComponent->GetNumericAttribute(UHealthAttributeSet::GetMaximumHealthAttribute());
			const float CurrentHealth = AbilitySystemComponent->GetNumericAttribute(UHealthAttributeSet::GetCurrentHealthAttribute());
				
			// Call the Blueprint Events to initialize the values.
			On_MaximumHealthChanged(MaxHealth, 0.0f, CurrentHealth / MaxHealth);
			On_CurrentHealthChanged(CurrentHealth, 0.0f, CurrentHealth / MaxHealth);
			On_HealthRegenerationChanged(AbilitySystemComponent->GetNumericAttribute(UHealthAttributeSet::GetHealthRegenerationAttribute()), 0.0f);
		}
		else
		{
			return false;
		}
	}
	
	// Bind Stamina attribute delegates if the Ability System Component has the required Attribute Set -and- we are listening for Stamina attributes.
	if (ListenForStaminaAttributeSetChanges)
	{
		if (AbilitySystemComponent->HasAttributeSetForAttribute(UStaminaAttributeSet::GetMaximumStaminaAttribute()))
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UStaminaAttributeSet::GetMaximumStaminaAttribute()).AddUObject(this, &UAbilitySystemWidget::MaximumStaminaChanged);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UStaminaAttributeSet::GetCurrentStaminaAttribute()).AddUObject(this, &UAbilitySystemWidget::CurrentStaminaChanged);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UStaminaAttributeSet::GetStaminaRegenerationAttribute()).AddUObject(this, &UAbilitySystemWidget::StaminaRegenerationChanged);

			const float MaxStamina = AbilitySystemComponent->GetNumericAttribute(UStaminaAttributeSet::GetMaximumStaminaAttribute());
			const float CurrentStamina = AbilitySystemComponent->GetNumericAttribute(UStaminaAttributeSet::GetCurrentStaminaAttribute());
		
			// Call the Blueprint Events to initialize the values.
			On_MaximumStaminaChanged(MaxStamina, 0.0f, CurrentStamina / MaxStamina);
			On_CurrentStaminaChanged(CurrentStamina, 0.0f, CurrentStamina / MaxStamina);
			On_StaminaRegenerationChanged(AbilitySystemComponent->GetNumericAttribute(UStaminaAttributeSet::GetStaminaRegenerationAttribute()), 0.0f);
		}
		else
		{
			return false;
		}
	}
	
	return true;
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