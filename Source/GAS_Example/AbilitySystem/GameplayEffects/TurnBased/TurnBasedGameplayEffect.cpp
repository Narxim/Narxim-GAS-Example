// Copyright 2021 Joseph "Narxim" Thigpen.


#include "TurnBasedGameplayEffect.h"

#include "GameplayEffectComponents/AssetTagsGameplayEffectComponent.h"
#include "GAS_Example/AbilitySystem/AbilitySystemComponent/TurnBased/TurnBasedGameplayEffectComponent.h"
#include "GAS_Example/AbilitySystem/Data/NativeGameplayTags.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "TurnBasedGameplayEffect"

UTurnBasedGameplayEffect::UTurnBasedGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
}

void UTurnBasedGameplayEffect::PostInitProperties()
{
	Super::PostInitProperties();
    
	// Force infinite duration regardless of designer input
	DurationPolicy = EGameplayEffectDurationType::Infinite;
    
	// Clear duration magnitude values
	DurationMagnitude = FGameplayEffectModifierMagnitude();

	// Always add the Turn based component
	FindOrAddComponent<UTurnBasedGameplayEffectComponent>();

	// Always add the Turn based asset tag.
	UAssetTagsGameplayEffectComponent& AssetTagComponent = FindOrAddComponent<UAssetTagsGameplayEffectComponent>();
	FInheritedTagContainer Tags = AssetTagComponent.GetConfiguredAssetTagChanges();
	Tags.AddTag(NativeGameplayTags::TAG_Effect_Type_TurnBased.GetTag());
	AssetTagComponent.SetAndApplyAssetTagChanges(Tags);
}

#if WITH_EDITOR
void UTurnBasedGameplayEffect::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
    
	// Force infinite duration if changed
	if(PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UGameplayEffect, DurationPolicy))
	{
		DurationPolicy = EGameplayEffectDurationType::Infinite;
	}
}

EDataValidationResult UTurnBasedGameplayEffect::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	// Validate duration policy
	if (DurationPolicy != EGameplayEffectDurationType::Infinite)
	{
		Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidDurationPolicyError",
			"[{0}] Requires Infinite Duration - Parent GE must use 'Infinite' duration policy to work with turn-based components"),
			FText::FromString(GetClass()->GetName())));
		Result = EDataValidationResult::Invalid;
	}

	// Check for required component
	if (FindComponent<UTurnBasedGameplayEffectComponent>() == nullptr)
	{
		Context.AddError(FText::FormatOrdered(LOCTEXT("MissingComponentError",
			"[{0}] Missing Required Component - Add a TurnBasedGameplayEffectComponent to enable turn-based behavior"),
			FText::FromString(GetClass()->GetName())));
		Result = EDataValidationResult::Invalid;
	}

	const UAssetTagsGameplayEffectComponent* const AssetTags = FindComponent<UAssetTagsGameplayEffectComponent>();
	if (!AssetTags || !AssetTags->GetConfiguredAssetTagChanges().Added.HasTag(NativeGameplayTags::TAG_Effect_Type_TurnBased.GetTag()))
	{
		Context.AddError(FText::FormatOrdered(LOCTEXT("AssetTag Error",
			"[{0}] Missing asset tag component, or TurnBased Tag is not added."),
			FText::FromString(GetClass()->GetName())));
		Result = EDataValidationResult::Invalid;
	}

	// Validate modifiers
	for (auto Modifier : Modifiers)
	{
		if (!Modifier.Attribute.IsValid())
		{
			Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidModifierAttributeError",
				"[{0}] Invalid Modifier Configuration\n"
				"Modifiers must either:\n"
				"• Set a valid Attribute\n"
				"• Be removed entirely if unused\n"
				"Empty attributes prevent GE application, including component effects"),
				FText::FromString(GetClass()->GetName())));
			Result = EDataValidationResult::Invalid;
		}
	}
	
	return Result;
}
#endif

#undef LOCTEXT_NAMESPACE