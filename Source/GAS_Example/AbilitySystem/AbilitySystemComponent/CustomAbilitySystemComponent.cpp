// Copyright 2021 Joseph "Narxim" Thigpen.

#include "CustomAbilitySystemComponent.h"
#include "GAS_Example/AbilitySystem/AttributeSets/LevelAttributeSet.h"
#include "GAS_Example/AbilitySystem/Data/NativeGameplayTags.h"
#include "GAS_Example/Characters/CharacterBase.h"


UCustomAbilitySystemComponent::UCustomAbilitySystemComponent()
{
	// Sets the Ability System Component to use "Mixed" replication mode. This will replicate minimal Gameplay Effects to Simulated Proxies and full info to everyone else.
	ReplicationMode = EGameplayEffectReplicationMode::Mixed;

	// Explicitly set the Ability System Component to replicate.
	SetIsReplicatedByDefault(true);
}

void UCustomAbilitySystemComponent::OnGameplayEffectAddedCallback(UAbilitySystemComponent* const ASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
{
	// Getting the Effect information, if they exist (which should work) and broadast them as "Added"
	if (const FActiveGameplayEffect* ActiveEffect = GetActiveGameplayEffect(Handle))
	{
		OnCustomGameplayEffectEventDelegate.Broadcast(ECustomEffectEventType::Added, *ActiveEffect);
	}
}

void UCustomAbilitySystemComponent::OnGameplayEffectRemovedCallback(const FActiveGameplayEffect& ActiveGameplayEffect)
{
	// Broadcast the removal of this effect with the CustomEffectEventType::Removed.
	OnCustomGameplayEffectEventDelegate.Broadcast(ECustomEffectEventType::Removed, ActiveGameplayEffect);
}

const UAttributeSet* UCustomAbilitySystemComponent::GetOrCreateAttributeSet(const TSubclassOf<UAttributeSet>& InAttributeSet)
{
	return GetOrCreateAttributeSubobject(InAttributeSet);
}

void UCustomAbilitySystemComponent::ChangeLevel_Implementation(const float DesiredLevel, const bool bInitialization /* = false */)
{
	// Get max and current level, and check that the desired new level is ok.
	const float MaxLevel = GetNumericAttribute(ULevelAttributeSet::GetMaximumLevelAttribute());
	const float CurrentLevel = GetNumericAttribute(ULevelAttributeSet::GetCurrentLevelAttribute());
	
	const float NewLevel = FMath::Clamp(DesiredLevel, 0.f, MaxLevel);

	if (FMath::IsNearlyEqual(NewLevel, CurrentLevel, 0.1f))
	{
		return;
	}

	// Set the new level.
	SetNumericAttributeBase(ULevelAttributeSet::GetCurrentLevelAttribute(), NewLevel);

	// Update all attributes that should be leveled up
	if (const ACharacterBase* const CharacterBase = GetCharacterBaseAvatar())
	{
		for (const TTuple<FGameplayAttribute, FCustomAttributeInitializer>& AttributeBaseValue : CharacterBase->GetAbilitySystemInitializationData().AttributeBaseValues)
		{
			// Don't touch the level itself.
			if (AttributeBaseValue.Key.GetAttributeSetClass() == ULevelAttributeSet::StaticClass())
			{
				continue;
			}

			// Don't touch Current Attribute if they are marked to not level up. They will be adjusted when the Max will (See the AttributeSets)
			if (!bInitialization && NoLevelAttribute.Contains(AttributeBaseValue.Key))
			{
				continue;
			}
			
			// Update the attribute using the new level. 
			if (HasAttributeSetForAttribute(AttributeBaseValue.Key))
			{
				SetNumericAttributeBase(AttributeBaseValue.Key, AttributeBaseValue.Value.Value.GetValueAtLevel(NewLevel));
			}
		}
	}

	// Update all GEs
	const int32 NewLevelInt = FMath::Floor(NewLevel);

	// Do not change the level of any GE with the NoLevel tag.
	FGameplayEffectQuery Query{};
	Query.EffectTagQuery.MakeQuery_MatchNoTags(NativeGameplayTags::TAG_Effect_NoLevel.GetTag().GetSingleTagContainer());
	
	// Note: Floats could be used, but need to create a new method.
	// Note 2: This method performance could be improved (Loop of Loops)
	SetActiveGameplayEffectLevelUsingQuery(Query, NewLevelInt);

	// Update all Abilities. They won't be Ended if active when this change happens.
	for (FGameplayAbilitySpec& ActivatableAbility : GetActivatableAbilities())
	{
		if (!ActivatableAbility.Level == NewLevelInt)
		{
			continue;
		}
		ActivatableAbility.Level = NewLevelInt;
		MarkAbilitySpecDirty(ActivatableAbility, false);
	} 
}

ACharacterBase* UCustomAbilitySystemComponent::GetCharacterBaseAvatar() const
{
	return Cast<ACharacterBase>(GetAvatarActor_Direct());
}

float UCustomAbilitySystemComponent::GetFilteredAttribute(const FGameplayAttribute Attribute, const FGameplayTagRequirements SourceTags, const FGameplayTagContainer TargetTags)
{
	return GetFilteredAttributeValue(Attribute, SourceTags, TargetTags);
}

void UCustomAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	/* JeanChene 2025/01/20:
	 * Copy from Lyra where both Pressed/Released works.
	 * It's not needed for Pressed
	 */
	
// 	// We don't support UGameplayAbility::bReplicateInputDirectly.
// 	// Use replicated events instead so that the WaitInputPress ability task works.
// 	if (Spec.IsActive())
// 	{
// PRAGMA_DISABLE_DEPRECATION_WARNINGS
// 		const UGameplayAbility* Instance = Spec.GetPrimaryInstance();
// 		FPredictionKey OriginalPredictionKey = Instance ? Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : Spec.ActivationInfo.GetActivationPredictionKey();
// PRAGMA_ENABLE_DEPRECATION_WARNINGS
//
// 		// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
// 		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, OriginalPredictionKey);
// 	}
}

void UCustomAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	/* JeanChene 2025/01/20:
	 * Copy from Lyra where both Pressed/Released works.
	 * Needed for Released as WaitInputRelease wasn't fired.
	 */
	
	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputRelease ability task works.
	if (Spec.IsActive())
	{
PRAGMA_DISABLE_DEPRECATION_WARNINGS
		const UGameplayAbility* Instance = Spec.GetPrimaryInstance();
		FPredictionKey OriginalPredictionKey = Instance ? Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : Spec.ActivationInfo.GetActivationPredictionKey();
PRAGMA_ENABLE_DEPRECATION_WARNINGS

		// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, OriginalPredictionKey);
	}
}

void UCustomAbilitySystemComponent::InitializeAbilitySystemData(const FAbilitySystemInitializationData& InitializationData, AActor* InOwningActor, AActor* InAvatarActor)
{
	if (AbilitySystemDataInitialized)
	{
		return;
	}
	
	AbilitySystemDataInitialized = true;

	// Set the Owning Actor and Avatar Actor. (Used throughout the Gameplay Ability System to get references etc.)
	InitAbilityActorInfo(InOwningActor, InAvatarActor);

	// Apply the Gameplay Tag container as loose Gameplay Tags. (These are not replicated by default and should be applied on both server and client respectively.)
	if (!InitializationData.GameplayTags.IsEmpty())
	{
		AddLooseGameplayTags(InitializationData.GameplayTags);
	}

	/** This is our entry point for other component to react to gameplay effect added and removed.
	 * Changes to the gameplay effects are handled differently (using their own delegate sets)
	 * -> FActiveGameplayEffectEvents* EventSet = ASC->GetActiveEffectEventSet(Handle);
	 * (See EffectWidgetControllerBase::InitializeController_Implementation)
	 */
	OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UCustomAbilitySystemComponent::OnGameplayEffectAddedCallback);
	ActiveGameplayEffects.OnActiveGameplayEffectRemovedDelegate.AddUObject(this, &UCustomAbilitySystemComponent::OnGameplayEffectRemovedCallback);
	
	// Check to see if we have authority. (Attribute Sets / Attribute Base Values / Gameplay Abilities / Gameplay Effects should only be added -or- set on authority and will be replicated to the client automatically.)
	if (!GetOwnerActor()->HasAuthority())
	{
		return;
	}
	
	// Grant Attribute Sets if the array isn't empty.
	if (!InitializationData.AttributeSets.IsEmpty())
	{
		for (const TSubclassOf<UAttributeSet> AttributeSetClass : InitializationData.AttributeSets)
		{
			GetOrCreateAttributeSet(AttributeSetClass);
		}
	}

	// Set base attribute values if the map isn't empty.
	if (!InitializationData.AttributeBaseValues.IsEmpty())
	{
		for (const TTuple<FGameplayAttribute, FCustomAttributeInitializer>& AttributeBaseValue : InitializationData.AttributeBaseValues)
		{
			if (HasAttributeSetForAttribute(AttributeBaseValue.Key))
			{
				SetNumericAttributeBase(AttributeBaseValue.Key, AttributeBaseValue.Value.Value.GetValueAtLevel(0.f));
			}
		}
	}

	// Grant Gameplay Abilities if the array isn't empty.
	if (!InitializationData.GameplayAbilities.IsEmpty())
	{
		for (const TSubclassOf<UGameplayAbility> GameplayAbility : InitializationData.GameplayAbilities)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(GameplayAbility, 0, INDEX_NONE, this);
			
			GiveAbility(AbilitySpec);
		}
	}

	// Apply Gameplay Effects if the array isn't empty.
	if (!InitializationData.GameplayEffects.IsEmpty())
	{
		for (const TSubclassOf<UGameplayEffect>& GameplayEffect : InitializationData.GameplayEffects)
		{
			if (!IsValid(GameplayEffect))
			{
				continue;
			}
			
			FGameplayEffectContextHandle EffectContextHandle = MakeEffectContext();
			EffectContextHandle.AddSourceObject(this);

			if (FGameplayEffectSpecHandle GameplayEffectSpecHandle = MakeOutgoingSpec(GameplayEffect, 1, EffectContextHandle); GameplayEffectSpecHandle.IsValid())
			{
				ApplyGameplayEffectSpecToTarget(*GameplayEffectSpecHandle.Data.Get(), this);
			}
		}
	}

	ChangeLevel(GetNumericAttribute(ULevelAttributeSet::GetCurrentLevelAttribute()), true);
}
