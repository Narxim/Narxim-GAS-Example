// Copyright 2021 Joseph "Narxim" Thigpen.


#include "GameplayAbility/CustomGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Kismet/KismetSystemLibrary.h"

UCustomGameplayAbility::UCustomGameplayAbility()
{
	// Sets the ability to default to Instanced Per Actor.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;	
}

void UCustomGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (ActivateAbilityOnGranted)
	{
		bool ActivatedAbility = ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}

void UCustomGameplayAbility::ApplyGameplayEffectToOwner(TSubclassOf<UGameplayEffect> EffectToAdd, float EffectLevel)
{
	FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle GameplayEffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(EffectToAdd, EffectLevel, EffectContextHandle);

	if (GameplayEffectSpecHandle.IsValid())
	{
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*GameplayEffectSpecHandle.Data.Get(), GetAbilitySystemComponentFromActorInfo());
	}
}

void UCustomGameplayAbility::ApplyGameplayEffectToTarget(TSubclassOf<UGameplayEffect> EffectToAdd, AActor* Target, float EffectLevel)
{
	FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle GameplayEffectSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(EffectToAdd, EffectLevel, EffectContextHandle);

	
	if (GameplayEffectSpecHandle.IsValid())
	{
		const bool ImplementsInterface = UKismetSystemLibrary::DoesImplementInterface(Target, UAbilitySystemInterface::StaticClass());

		if (ImplementsInterface)
		{
			IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(Target);
		
			GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*GameplayEffectSpecHandle.Data.Get(), Interface->GetAbilitySystemComponent());
		}
	}
}

float UCustomGameplayAbility::GetModifiedAbilityStrength()
{
	float BaseStrength = BaseAbilityStrength.GetValueAtLevel(GetAbilityLevel());

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	
	for (FAbilityModifierStruct Modifier : AbilityModifiers)
	{
		const float AttributeValue = AbilitySystemComponent->GetNumericAttribute(Modifier.BackingAttribute);

		BaseStrength += AttributeValue * Modifier.AttributeModifier;
	}

	return BaseStrength;	
}