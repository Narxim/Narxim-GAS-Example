// Copyright 2021 Joseph "Narxim" Thigpen.


#include "TurnBasedGameplayEffect.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "TurnBasedGameplayEffectComponent.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffectComponents/AssetTagsGameplayEffectComponent.h"
#include "GAS_Example/AbilitySystem/AttributeSets/TurnBasedAttributeSet.h"
#include "GAS_Example/AbilitySystem/Data/AbilitySystemData.h"
#include "GAS_Example/AbilitySystem/Data/NativeGameplayTags.h"
#include "GAS_Example/AbilitySystem/Data/TurnBasedNativeGameplayTags.h"
#include "GAS_Example/AbilitySystem/FunctionLibrary/TurnBasedAbilitySystemBlueprintLibrary.h"
#include "GAS_Example/Game/TurnBased/TurnExampleGameStateBase.h"
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
	Tags.AddTag(TurnBasedNativeGameplayTags::TAG_Effect_Type_TurnBased.GetTag());
	AssetTagComponent.SetAndApplyAssetTagChanges(Tags);

	// For now, we limit turn based effect to only one stack, not refreshable.
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackLimitCount = 1;
	bDenyOverflowApplication = true;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
}

FGameplayEffectContextHandle UTurnBasedGameplayEffect::MakeTurnBasedEffectContext(
	const float Level,
	UAbilitySystemComponent* ASC,
	const UGameplayAbility* InGameplayAbility,
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo ActorInfo,
	const TSubclassOf<UTurnBasedGameplayEffect>& TurnBasedEffectClass)
{

	FGameplayEffectContextHandle Context{};
	if (!TurnBasedEffectClass)
	{
		return Context;
	}
	
	FGameplayTagContainer SourceTags{};

	if (ASC)
	{
		SourceTags.AppendTags(ASC->GetOwnedGameplayTags());
	}
	
	if (InGameplayAbility)
	{
		Context = InGameplayAbility->MakeEffectContext(Handle, &ActorInfo);
		SourceTags.AppendTags(InGameplayAbility->GetAssetTags());
	}
	else if (ASC)
	{
		Context = ASC->MakeEffectContext();
	} else
	{
		return Context;
	}

	SourceTags.AppendTags(TurnBasedEffectClass.GetDefaultObject()->GetAssetTags());

	UpdateTurnBasedEffectContext(Context, Level, TurnBasedEffectClass, ASC, SourceTags);

	return Context;
}

bool UTurnBasedGameplayEffect::UpdateTurnBasedEffectContext(
	FGameplayEffectContextHandle& EffectContextHandle,
	const float Level,
	const TSubclassOf<UTurnBasedGameplayEffect>& TurnBasedEffectClass,
	UAbilitySystemComponent* ASC,
	const FGameplayTagContainer& SourceAggregatedTags)
{
	if (!TurnBasedEffectClass || !ASC)
	{
		return false;
	}

	const UTurnBasedGameplayEffectComponent* const TurnComponent = UTurnBasedAbilitySystemBlueprintLibrary::ExtractTurnBasedEffectComponentFromGE(TurnBasedEffectClass.GetDefaultObject());
	if (!TurnComponent)
	{
		return false;
	}

	const ATurnExampleGameStateBase* const TurnSystem = UTurnBasedAbilitySystemBlueprintLibrary::GetTurnSystem(EffectContextHandle.GetSourceObject() ? EffectContextHandle.GetSourceObject() : EffectContextHandle.GetInstigator());
	if (!TurnSystem)
	{
		return false;
	}

	FCustomGameplayEffectContext* const ContextData = static_cast<FCustomGameplayEffectContext*>(EffectContextHandle.Get());
	if (!ContextData)
	{
		return false;
	}
	
	const float ValidLevel = FMath::Min(Level, 0.f);

	bool bEvalSuccess = false;
	FGameplayTagContainer AttributeTags = SourceAggregatedTags;

	const int32 InhibitTurnBase = TurnComponent->InhibitionDelayTurns.AsInteger(ValidLevel);
	AttributeTags.AddTag(TurnBasedNativeGameplayTags::TAG_Effect_Modifier_InhibitionTurn.GetTag());
	int32 InhibTurn = UAbilitySystemBlueprintLibrary::EvaluateAttributeValueWithTagsAndBase(
		ASC,
		UTurnBasedAttributeSet::GetTurnBasedModifierAttribute(),
		AttributeTags,
		FGameplayTagContainer(),
		InhibitTurnBase,
		bEvalSuccess);

	if (!bEvalSuccess)
	{
		InhibTurn = InhibitTurnBase;
	}

	const int32 DurationTurnsBase = TurnComponent->DurationTurns.AsInteger(ValidLevel);
	bEvalSuccess = false;
	AttributeTags = SourceAggregatedTags;
	AttributeTags.AddTag(TurnBasedNativeGameplayTags::TAG_Effect_Modifier_DurationTurn.GetTag());
	
	int32 DurationTurns = UAbilitySystemBlueprintLibrary::EvaluateAttributeValueWithTagsAndBase(
		ASC,
		UTurnBasedAttributeSet::GetTurnBasedModifierAttribute(),
		AttributeTags,
		FGameplayTagContainer(),
		TurnComponent->DurationTurns.AsInteger(ValidLevel),
		bEvalSuccess);

	if (!bEvalSuccess)
	{
		DurationTurns = DurationTurnsBase;
	}

	const int32 MaxPeriodicApplicationsBase = TurnComponent->MaxPeriodicApplications.AsInteger(ValidLevel);
	bEvalSuccess = false;
	AttributeTags = SourceAggregatedTags;
	AttributeTags.AddTag(TurnBasedNativeGameplayTags::TAG_Effect_Modifier_MaxPeriodicApplications.GetTag());
	
	int32 MaxPeriodicApplications = UAbilitySystemBlueprintLibrary::EvaluateAttributeValueWithTagsAndBase(
		ASC,
		UTurnBasedAttributeSet::GetTurnBasedModifierAttribute(),
		AttributeTags,
		FGameplayTagContainer(),
		MaxPeriodicApplicationsBase,
		bEvalSuccess);
	
	if (!bEvalSuccess)
	{
		MaxPeriodicApplications = MaxPeriodicApplicationsBase;
	}
	
	auto& MutableContextData = ContextData->GetMutableAllCustomContextData();
	MutableContextData.Emplace(
		FInstancedStruct::Make(
			FCustomContextData_TurnBaseEffect(
				TurnSystem->GetCurrentTurn(),
				FMath::Max(InhibTurn, 0),
				FMath::Max(DurationTurns, 1),
				FMath::Max(MaxPeriodicApplications, 1)
				)
			)
		);

	return true;
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
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		// CDO, cannot validate it.
		return EDataValidationResult::Valid;
	}
	
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
	if (!AssetTags || !AssetTags->GetConfiguredAssetTagChanges().Added.HasTag(TurnBasedNativeGameplayTags::TAG_Effect_Type_TurnBased.GetTag()))
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