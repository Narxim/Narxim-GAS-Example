// Copyright 2021 Joseph "Narxim" Thigpen.


#include "CooldownGameplayEffectBase.h"

#include "GameplayEffectComponents/AssetTagsGameplayEffectComponent.h"
#include "GAS_Example/AbilitySystem/AttributeSets/AbilitiesAttributeSet.h"
#include "GAS_Example/AbilitySystem/Data/NativeGameplayTags.h"

UCooldownGameplayEffectBase::UCooldownGameplayEffectBase()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackLimitCount = 1;
}

void UCooldownGameplayEffectBase::PostLoad()
{
	Super::PostLoad();
	ConfigureEffect();
}

void UCooldownGameplayEffectBase::PostInitProperties()
{
	Super::PostInitProperties();
	ConfigureEffect();
}

void UCooldownGameplayEffectBase::ConfigureEffect()
{
	UAssetTagsGameplayEffectComponent& AssetTagComponent = FindOrAddComponent<UAssetTagsGameplayEffectComponent>();
	FInheritedTagContainer InheritedTags{};
	InheritedTags.AddTag(NativeGameplayTags::TAG_Effect_Type_Cooldown.GetTag());
	AssetTagComponent.SetAndApplyAssetTagChanges(InheritedTags);

	FAttributeBasedFloat AttributeBaseCooldown{};
	AttributeBaseCooldown.AttributeCalculationType = EAttributeBasedFloatCalculationType::AttributeMagnitude;
	AttributeBaseCooldown.BackingAttribute.AttributeToCapture = UAbilitiesAttributeSet::GetCooldownMultiplierAttribute();
	AttributeBaseCooldown.BackingAttribute.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;

	DurationMagnitude = AttributeBaseCooldown;
}
