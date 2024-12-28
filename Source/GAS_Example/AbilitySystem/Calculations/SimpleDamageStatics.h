// Copyright 2021 Joseph "Narxim" Thigpen.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GAS_Example/AbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "GAS_Example/AbilitySystem/AttributeSets/ResistanceAttributeSet.h"

struct FSimpleCustomDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaximumHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	
	FSimpleCustomDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UResistanceAttributeSet, Resistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, CurrentHealth, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, MaximumHealth, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, Damage, Target, false);
	}

	static const FSimpleCustomDamageStatics& DamageStatics()
	{
		static FSimpleCustomDamageStatics DmgStatics;
		return DmgStatics;
	}
};
