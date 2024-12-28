// Copyright 2021 Joseph "Narxim" Thigpen.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GAS_Example/AbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "GAS_Example/AbilitySystem/AttributeSets/ResistanceAttributeSet.h"

struct FCustomDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaximumHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ReceivedBleed);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Bleeding);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	
	FCustomDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UResistanceAttributeSet, Resistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, CurrentHealth, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, MaximumHealth, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, ReceivedBleed, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, Bleeding, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, Damage, Target, false);
	}

	static const FCustomDamageStatics& DamageStatics()
	{
		static FCustomDamageStatics DmgStatics;
		return DmgStatics;
	}
};
