// Copyright 2021 Joseph "Narxim" Thigpen.


#include "CRLDamageExecution.h"

#include "CRLAbility.h"
#include "DamageStatics.h"
#include "NativeGameplayTags.h"
#include "CustomRogueLiteTools/Components/CRLActorComponent.h"
#include "CustomRogueLiteTools/Interface/CRLActorInterface.h"
#include "GAS_Example/AbilitySystem/AbilitySystemComponent/CustomAbilitySystemComponent.h"

#define REPEAT(x) for(int i = x; i--;)

const FCRLDamageStatics* UCRLDamageExecution::DamageStatics = nullptr;

UCRLDamageExecution::UCRLDamageExecution():Super()
{
	if (!DamageStatics)
	{
		DamageStatics = &FCRLDamageStatics::CRLDamageStatics();
	}
	
	RelevantAttributesToCapture.Add(DamageStatics->DamageDef);
	RelevantAttributesToCapture.Add(DamageStatics->DamageMultiplierDef);
	RelevantAttributesToCapture.Add(DamageStatics->ResistanceMultiplierDef);
	RelevantAttributesToCapture.Add(DamageStatics->MultiplierDef);
	RelevantAttributesToCapture.Add(DamageStatics->CriticalChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics->CriticalDamageMultiplierDef);
}

void UCRLDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	if (!DamageStatics)
	{
		DamageStatics = &FCRLDamageStatics::CRLDamageStatics();
	}
	
	UCustomAbilitySystemComponent* const TargetAbilitySystemComponent = Cast<UCustomAbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());
	if (!TargetAbilitySystemComponent)
	{
		return;
	}

	// Feel free to uncomment any of those lines to have access to other objects.
	UCustomAbilitySystemComponent* const SourceAbilitySystemComponent = Cast<UCustomAbilitySystemComponent>(ExecutionParams.GetSourceAbilitySystemComponent());

	AActor* const SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	AActor* const TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	if (!SourceActor || !TargetActor || !Spec.CapturedSourceTags.GetAggregatedTags())
	{
		return;
	}
	
	// ACharacterBase* const SourceCharacter = Cast<ACharacterBase>(SourceActor);
	// ACharacterBase* const TargetCharacter = Cast<ACharacterBase>(TargetActor);
	
	FAggregatorEvaluateParameters EvaluationParameters{};
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	SourceCLRComponent = ICRLActorInterface::Execute_GetCRLActorComponent(SourceActor);
	if (!SourceCLRComponent)
	{
		return;
	}
	
	AttributeCollection.SourceTags = *EvaluationParameters.SourceTags;
	AttributeCollection.TargetTags = *EvaluationParameters.TargetTags;
	AttributeCollection.TargetASC = TargetAbilitySystemComponent;
	AttributeCollection.TargetActor = TargetActor;
	AttributeCollection.InstigatorActor = SourceActor;

	AttributeCollection.CritRand = FMath::FRand();

	/*
	 * 1 - Calculate Damage
	 * 2 - Calculate Pre-Armor Conditions
	 * 3 - Calculate Armor
	 * 4 - Calculate Post-Armor Conditions
	 * 4 - Calculate final damage =
	 * */

	FCRLChangedValue& DamageStruct = AttributeCollection.ChangedAttributeCollection.Emplace(CRLTags::TAG_CRL_Identifier_Damage.GetTag());
	FCRLChangedValue& ResistanceStruct = AttributeCollection.ChangedAttributeCollection.Emplace(CRLTags::TAG_CRL_Identifier_Resistance.GetTag());
	ResistanceStruct.Value = 1.f;
	FCRLChangedValue& CriticDamageStruct = AttributeCollection.ChangedAttributeCollection.Emplace(CRLTags::TAG_CRL_Identifier_Critic.GetTag());
	
	ProcessModifierEvent(ECRLModifierEvent::PreDamageCalculation);

	// Get damage and Critical Chance
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics->DamageDef, EvaluationParameters, DamageStruct.Value);
	DamageStruct.Value = FMath::Max(DamageStruct.Value, 0.f);

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics->CriticalDamageMultiplierDef, EvaluationParameters, CriticDamageStruct.Multiplier);
	CriticDamageStruct.Multiplier = FMath::Max(CriticDamageStruct.Multiplier, 0.f);
	
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics->CriticalChanceDef, EvaluationParameters, CriticDamageStruct.Value);
	CriticDamageStruct.Value = FMath::Clamp(CriticDamageStruct.Value, 0.f, 1.f);

	ProcessModifierEvent(ECRLModifierEvent::PostDamageCalculation);
	
	FGameplayTagContainer DamageMultiplierSource = *EvaluationParameters.SourceTags;
	if ( EvaluationParameters.SourceTags->HasTagExact(CRLTags::TAG_CRL_Buff_Backstab.GetTag()) &&
		!EvaluationParameters.TargetTags->HasTagExact(CRLTags::TAG_CRL_Immunity_Backstab.GetTag()))
	{
		const float Direction = SourceActor->GetActorForwardVector().Dot(TargetActor->GetActorForwardVector());
		if (Direction > 0.f)
		{
			// Looking in the same direction, meaning you're in his back
			DamageMultiplierSource.AddTag(CRLTags::TAG_CRL_Modifier_Backstab.GetTag());
		}
	}

	ProcessModifierEvent(ECRLModifierEvent::PreMitigation);
	
	// This number will be multiplied by the damage at the end.
	float DamageResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics->ResistanceMultiplierDef, EvaluationParameters, DamageResistance);
	ResistanceStruct.Value += DamageResistance;

	float DamageMultiplierTmp = 1.f;
	EvaluationParameters.SourceTags = &DamageMultiplierSource;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics->DamageMultiplierDef, EvaluationParameters, DamageMultiplierTmp);
	DamageMultiplierTmp = FMath::Max(DamageMultiplierTmp, 0.f);
	DamageStruct.Multiplier += (DamageMultiplierTmp - 1.f);

	ProcessModifierEvent(ECRLModifierEvent::PostMitigation);

	// Calculate the final damage
	float FinalDamage = DamageStruct.GetMagnitude() * ResistanceStruct.GetMagnitude();
	
	if (AttributeCollection.IsCriticalHit())
	{
		FinalDamage *= FMath::Max(1.f + CriticDamageStruct.Multiplier, 1.f);
	}

	// Might have other damage to add, new if scope instead of early return
	if (FinalDamage > 0.f)
	{
		// Output the final damage to our player.
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				DamageStatics->CurrentHealthProperty,
				EGameplayModOp::Additive,
				-FinalDamage
				)
		);
	}
}

void UCRLDamageExecution::ProcessModifierEvent(ECRLModifierEvent Event) const
{
	const UCRLActorComponent* CLRComponent = SourceCLRComponent;
	ECRLTargetType TargetType = ECRLTargetType::Instigator;
	REPEAT(2)
	{
		if (!CLRComponent)
		{
			continue;
		}
		
		const FCRLModifierDefinition* Modifiers = CLRComponent->GetModifierDefinitionsForEvent(Event);
		if (!Modifiers)
		{
			return;
		}
		
		for (const TObjectPtr<UCRLAbility>& Ability : Modifiers->GetAbilities(TargetType))
		{
			if (!Ability)
			{
				continue;
			}

			Ability->ProcessChangedAttributeCollection(TargetType, AttributeCollection);
		}

		if (!TargetCLRComponent)
		{
			break;
		}
		CLRComponent = TargetCLRComponent;
		TargetType = ECRLTargetType::Target;
	}
}

bool UCRLDamageExecution::GetModifierFromAttributeCollection(FCRLChangedAttributeCollection& Collection, const FGameplayTag Identifier, FCRLChangedValue& OutModifier)
{
	if (FCRLChangedValue* const Modifier = Collection.ChangedAttributeCollection.Find(Identifier))
	{
		OutModifier = *Modifier;
		return true;
	}

	return false;
}
