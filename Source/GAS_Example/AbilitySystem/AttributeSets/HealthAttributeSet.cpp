// Copyright 2021 Joseph "Narxim" Thigpen.


#include "HealthAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"


UHealthAttributeSet::UHealthAttributeSet()
{
	MaximumHealth = 1.0f;
	CurrentHealth = 1.0f;
	HealthRegeneration = 0.0f;
}

void UHealthAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params{};
	Params.bIsPushBased = true;
	Params.Condition = COND_None;

	// Replicated to all
	DOREPLIFETIME_WITH_PARAMS_FAST(UHealthAttributeSet, CurrentHealth, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(UHealthAttributeSet, MaximumHealth, Params);

	// Only Owner
	Params.Condition = COND_OwnerOnly;
	DOREPLIFETIME_WITH_PARAMS_FAST(UHealthAttributeSet, HealthRegeneration, Params);
}

void UHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaximumHealthAttribute())
	{
		AdjustAttributeForMaxChange(CurrentHealth, MaximumHealth, NewValue, GetCurrentHealthAttribute());
	}
}

void UHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Store a local copy of the amount of Damage done and clear the Damage attribute.
		const float LocalDamageDone = GetDamage();

		SetDamage(0.f);
	
		if (LocalDamageDone > 0.0f)
		{
			// Apply the Health change and then clamp it.
			const float NewHealth = GetCurrentHealth() - LocalDamageDone;

			SetCurrentHealth(FMath::Clamp(NewHealth, 0.0f, GetMaximumHealth()));
		}
	}

	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		// Store a local copy of the amount of Healing done and clear the Healing attribute.
		const float LocalHealingDone = GetHealing();

		SetHealing(0.f);
	
		if (LocalHealingDone > 0.0f)
		{
			// Apply the Health change and then clamp it.
			const float NewHealth = GetCurrentHealth() + LocalHealingDone;

			SetCurrentHealth(FMath::Clamp(NewHealth, 0.0f, GetMaximumHealth()));
		}
	}
	
	else if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		SetCurrentHealth(FMath::Clamp(GetCurrentHealth(), 0.0f, GetMaximumHealth()));
	}

	else if (Data.EvaluatedData.Attribute == GetHealthRegenerationAttribute())
	{
		SetHealthRegeneration(FMath::Clamp(GetHealthRegeneration(), 0.0f, GetMaximumHealth()));
	}
}

void UHealthAttributeSet::OnRep_CurrentHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, CurrentHealth, OldValue);
}

void UHealthAttributeSet::OnRep_MaximumHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, MaximumHealth, OldValue);
}

void UHealthAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, HealthRegeneration, OldValue);
}