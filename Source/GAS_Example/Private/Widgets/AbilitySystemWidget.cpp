// Copyright 2021 Joseph "Narxim" Thigpen.

#include "Widgets/AbilitySystemWidget.h"
#include "AbilitySystemGlobals.h"
#include "Player/PlayerStateBase.h"
#include "AbilitySystemComponent/CustomAbilitySystemComponent.h"
#include "AttributeSets/HealthAttributeSet.h"
#include "AttributeSets/StaminaAttributeSet.h"

bool UAbilitySystemWidget::InitializeAbilitySystemWidget(UAbilitySystemComponent* AbilitySystemComponent)
{
	// Reset the Attribute change Delegate Handles in case they are already bound.
	CurrentHealthChangeDelegate.Reset();
	MaximumHealthChangeDelegate.Reset();
	HealthRegenerationChangeDelegate.Reset();
	CurrentStaminaChangeDelegate.Reset();
	MaximumStaminaChangeDelegate.Reset();
	StaminaRegenerationChangeDelegate.Reset();

	// Reset the Ability System Component pointer if its already set.
	AbilitySystem = Cast<UCustomAbilitySystemComponent>(AbilitySystemComponent);

	if (!AbilitySystem.Get())
	{
		// Check that the Player State Base class is reachable.
		if (const APlayerStateBase* PlayerStateBase = Cast<APlayerStateBase>(GetOwningPlayerState()))
		{
			// Set the Ability System Component reference.
			AbilitySystem = PlayerStateBase->AbilitySystemComponent;
		}

		// If the Ability System Component is not valid try using the Owning Player Pawn.
		// For the case where this Widget is on an Actor who does not have a Player State or the previous method failed.
		if (!AbilitySystem.Get())
		{
			if (const APawn* Pawn = GetOwningPlayerPawn())
			{
				// Try setting the pointer again.
				AbilitySystem = Cast<UCustomAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn));
			}
		}

		// Return false if the Ability System Component pointer is still null.
		if (!AbilitySystem.Get())
		{
			return false;
		}
	}
		
	// Bind the Attribute Change Delegates.
	BindAbilitySystemDelegates();

	return true;
}

void UAbilitySystemWidget::BindAbilitySystemDelegates()
{
	// Bind Health Attribute delegates if the Attribute set is owned by the Ability System Component.
	if (AbilitySystem->HasAttributeSetForAttribute(UHealthAttributeSet::GetMaximumHealthAttribute()))
	{
		MaximumHealthChangeDelegate = AbilitySystem->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetMaximumHealthAttribute()).AddUObject(this, &UAbilitySystemWidget::MaximumHealthChanged);
		CurrentHealthChangeDelegate = AbilitySystem->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetCurrentHealthAttribute()).AddUObject(this, &UAbilitySystemWidget::CurrentHealthChanged);
		HealthRegenerationChangeDelegate = AbilitySystem->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetHealthRegenerationAttribute()).AddUObject(this, &UAbilitySystemWidget::HealthRegenerationChanged);

		const float MaxHealth = AbilitySystem->GetNumericAttribute(UHealthAttributeSet::GetMaximumHealthAttribute());
		const float CurrentHealth = AbilitySystem->GetNumericAttribute(UHealthAttributeSet::GetCurrentHealthAttribute());
				
		// Call the Blueprint Events to initialize the values.
		On_MaximumHealthChanged(MaxHealth, 0.0f, CurrentHealth / MaxHealth);
		On_CurrentHealthChanged(CurrentHealth, 0.0f, CurrentHealth / MaxHealth);
		On_HealthRegenerationChanged(AbilitySystem->GetNumericAttribute(UHealthAttributeSet::GetHealthRegenerationAttribute()), 0.0f);
	}

	// Bind Stamina Attribute delegates if the Attribute set is owned by the Ability System Component.
	if (AbilitySystem->HasAttributeSetForAttribute(UStaminaAttributeSet::GetMaximumStaminaAttribute()))
	{
		MaximumStaminaChangeDelegate = AbilitySystem->GetGameplayAttributeValueChangeDelegate(UStaminaAttributeSet::GetMaximumStaminaAttribute()).AddUObject(this, &UAbilitySystemWidget::MaximumStaminaChanged);
		CurrentStaminaChangeDelegate = AbilitySystem->GetGameplayAttributeValueChangeDelegate(UStaminaAttributeSet::GetCurrentStaminaAttribute()).AddUObject(this, &UAbilitySystemWidget::CurrentStaminaChanged);
		StaminaRegenerationChangeDelegate = AbilitySystem->GetGameplayAttributeValueChangeDelegate(UStaminaAttributeSet::GetStaminaRegenerationAttribute()).AddUObject(this, &UAbilitySystemWidget::StaminaRegenerationChanged);

		const float MaxStamina = AbilitySystem->GetNumericAttribute(UStaminaAttributeSet::GetMaximumStaminaAttribute());
		const float CurrentStamina = AbilitySystem->GetNumericAttribute(UStaminaAttributeSet::GetCurrentStaminaAttribute());
		
		// Call the Blueprint Events to initialize the values.
		On_MaximumStaminaChanged(MaxStamina, 0.0f, CurrentStamina / MaxStamina);
		On_CurrentStaminaChanged(CurrentStamina, 0.0f, CurrentStamina / MaxStamina);
		On_StaminaRegenerationChanged(AbilitySystem->GetNumericAttribute(UStaminaAttributeSet::GetStaminaRegenerationAttribute()), 0.0f);
	}
}

void UAbilitySystemWidget::MaximumHealthChanged(const FOnAttributeChangeData& Data)
{
	const float CurrentHealth = AbilitySystem->GetNumericAttribute(UHealthAttributeSet::GetCurrentHealthAttribute());

	On_MaximumHealthChanged(Data.NewValue, Data.OldValue, CurrentHealth / Data.NewValue);
}

void UAbilitySystemWidget::CurrentHealthChanged(const FOnAttributeChangeData& Data)
{
	const float MaxHealth = AbilitySystem->GetNumericAttribute(UHealthAttributeSet::GetMaximumHealthAttribute());

	On_CurrentHealthChanged(Data.NewValue, Data.OldValue, Data.NewValue / MaxHealth);
}

void UAbilitySystemWidget::HealthRegenerationChanged(const FOnAttributeChangeData& Data)
{
	On_HealthRegenerationChanged(Data.NewValue, Data.OldValue);
}

void UAbilitySystemWidget::MaximumStaminaChanged(const FOnAttributeChangeData& Data)
{
	const float CurrentStamina = AbilitySystem->GetNumericAttribute(UStaminaAttributeSet::GetCurrentStaminaAttribute());

	On_MaximumStaminaChanged(Data.NewValue, Data.OldValue, CurrentStamina / Data.NewValue);
}

void UAbilitySystemWidget::CurrentStaminaChanged(const FOnAttributeChangeData& Data)
{
	const float MaxStamina = AbilitySystem->GetNumericAttribute(UStaminaAttributeSet::GetMaximumStaminaAttribute());

	On_CurrentStaminaChanged(Data.NewValue, Data.OldValue, Data.NewValue / MaxStamina);
}

void UAbilitySystemWidget::StaminaRegenerationChanged(const FOnAttributeChangeData& Data)
{
	On_StaminaRegenerationChanged(Data.NewValue, Data.OldValue);
}