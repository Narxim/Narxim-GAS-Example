// Copyright 2021 Joseph "Narxim" Thigpen.
#pragma once

#include "CoreMinimal.h"
#include "CRLAttributeSet.h"
#include "UObject/Object.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GAS_Example/AbilitySystem/AttributeSets/HealthAttributeSet.h"

struct FCRLDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaximumHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ReceivedBleed);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Bleeding);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Multiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistanceMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalDamageMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalChance);
	
	FCRLDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, CurrentHealth, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, MaximumHealth, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, ReceivedBleed, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCRLAttributeSet, ResistanceMultiplier, Target, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, Bleeding, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, Damage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCRLAttributeSet, Multiplier, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCRLAttributeSet, DamageMultiplier, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCRLAttributeSet, CriticalDamageMultiplier, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCRLAttributeSet, CriticalChance, Source, false);
	}

	static const FCRLDamageStatics& CRLDamageStatics()
	{
		static FCRLDamageStatics DmgStatics;
		return DmgStatics;
	}
};
