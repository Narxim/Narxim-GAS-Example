// Copyright 2021 Joseph "Narxim" Thigpen.


#include "LevelAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GAS_Example/AbilitySystem/AbilitySystemComponent/CustomAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

ULevelAttributeSet::ULevelAttributeSet()
{
	MaximumLevel = 0.0f;
	CurrentLevel = 0.0f;
}

void ULevelAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void ULevelAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void ULevelAttributeSet::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);

	if (Attribute != GetCurrentLevelAttribute())
	{
		return;
	}

	if (UCustomAbilitySystemComponent* const ASC = Cast<UCustomAbilitySystemComponent>(GetOwningAbilitySystemComponent()))
	{
		ASC->ChangeLevel(NewValue);
	}
}

void ULevelAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params{};
	Params.bIsPushBased = true;
	Params.Condition = COND_None;

	// Replicated to all
	DOREPLIFETIME_WITH_PARAMS_FAST(ULevelAttributeSet, CurrentLevel, Params);
	
	// Only Owner
	Params.Condition = COND_OwnerOnly;
	DOREPLIFETIME_WITH_PARAMS_FAST(ULevelAttributeSet, MaximumLevel, Params);
}

void ULevelAttributeSet::OnRep_CurrentLevel(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULevelAttributeSet, CurrentLevel, OldValue);
}

void ULevelAttributeSet::OnRep_MaximumLevel(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULevelAttributeSet, MaximumLevel, OldValue);
}