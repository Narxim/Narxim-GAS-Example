// Copyright 2021 Joseph "Narxim" Thigpen.


#include "Characters/CharacterBase.h"
#include "AbilitySystemComponent/CustomAbilitySystemComponent.h"
#include "GameplayAbility/CustomGameplayAbility.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void ACharacterBase::GiveDefaultAbilities()
{
	if (AbilitySystemComponent->DefaultAbilitiesGiven || GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	
	for (TSubclassOf<UCustomGameplayAbility>& DefaultAbility : DefaultAbilities)
	{
		if (IsValid(DefaultAbility))
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility, 1, static_cast<int32>(DefaultAbility.GetDefaultObject()->InputBinding), this));
		}
	}

	AbilitySystemComponent->DefaultAbilitiesGiven = true;
}

void ACharacterBase::ApplyDefaultEffects()
{
	if (AbilitySystemComponent->DefaultEffectsApplied || GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	for (TSubclassOf<UGameplayEffect> Effect : DefaultEffects)
	{
		if (IsValid(Effect))
		{
			FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
			EffectContextHandle.AddSourceObject(this);

			FGameplayEffectSpecHandle GameplayEffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, EffectContextHandle);

			if (GameplayEffectSpecHandle.IsValid())
			{
				AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*GameplayEffectSpecHandle.Data.Get(), AbilitySystemComponent.Get());
			}
		}
	}	

	AbilitySystemComponent->DefaultEffectsApplied = true;
}

void ACharacterBase::OnDeath()
{
	// Call "Event On Death" in Blueprint.
	On_Death();
}