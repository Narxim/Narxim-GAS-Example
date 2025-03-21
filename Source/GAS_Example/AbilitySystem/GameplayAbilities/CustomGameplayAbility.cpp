// Copyright 2021 Joseph "Narxim" Thigpen.


#include "CustomGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "EnhancedInputComponent.h"
#include "Engine/AssetManager.h"
#include "GAS_Example/Characters/CharacterBase.h"

#define LOCTEXT_NAMESPACE "GameplayAbility"

UCustomGameplayAbility::UCustomGameplayAbility()
{
	// Sets the ability to default to Instanced Per Actor.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;	
}

UAnimMontage* UCustomGameplayAbility::GetMontageToUse_Implementation() const
{
	return MontageHardPtr.Get();
}

bool UCustomGameplayAbility::PlayConfiguredMontage()
{
	UAbilitySystemComponent* const AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Ensured();
	
	if (AbilitySystemComponent && AbilitySystemComponent->IsAnimatingAbility(this))
	{
		// Already playing
		return false;
	}

	const FGameplayAbilityActorInfo* const ActorInfo = GetCurrentActorInfo();
	if (!ActorInfo)
	{
		return false;
	}

	return PlayMontage(CurrentSpecHandle, ActorInfo, GetCurrentActivationInfoRef(), ActorInfo->GetAnimInstance());
}

bool UCustomGameplayAbility::PlayMontage(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, UAnimInstance* AnimInstance)
{
	UAnimMontage* const MontageToUse = GetMontageToUse();
	UAbilitySystemComponent* const ASC = GetAbilitySystemComponentFromActorInfo();
	if (!AnimInstance || !MontageToUse || !ASC)
	{
		return false;
	}

	SetCurrentMontage(MontageToUse);

	ACharacter* const Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (Character && (Character->GetLocalRole() == ROLE_Authority ||
					  (Character->GetLocalRole() == ROLE_AutonomousProxy && GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
	{
		Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
	}
	
	const float MontageDuration = ASC->PlayMontage(this, ActivationInfo, CurrentMontage.Get(), GetMontagePlayRate(), GetMontageSectionToPlay(), GetMontageStartTime());

	if (MontageDuration <= 0.f)
	{
		return false;
	}
	
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, MontageToUse);

	K0_PlayMontage(ActivationInfo, AnimInstance);

	OnMontageStarted();
	
	return true;
}

void UCustomGameplayAbility::K0_PlayMontage_Implementation(const FGameplayAbilityActivationInfo ActivationInfo,	UAnimInstance* AnimInstance)
{
}

void UCustomGameplayAbility::OnMontageEnded(UAnimMontage* MontageEnded, bool bInterrupted)
{
	SetCurrentMontage(nullptr);
	
	K0_OnMontageEnded(MontageEnded, bInterrupted);
	
	if (bEndAbilityOnMontageEnd)
	{
		K2_EndAbility();
	}
}

void UCustomGameplayAbility::K0_OnMontageEnded_Implementation(UAnimMontage* MontageEnded, bool bInterrupted)
{
}

float UCustomGameplayAbility::GetMontagePlayRate_Implementation()
{
	return 1.f;
}

FName UCustomGameplayAbility::GetMontageSectionToPlay_Implementation()
{
	return NAME_None;
}

float UCustomGameplayAbility::GetMontageStartTime_Implementation()
{
	return 0.f;
}

void UCustomGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	// Set the "Avatar Character" reference.
	AvatarCharacter = Cast<ACharacterBase>(ActorInfo->AvatarActor);

	// Set up Bindings for Enhanced Input.
	SetupEnhancedInputBindings(ActorInfo, Spec);

	// Try to Activate immediately if "Activate Ability On Granted" is true.
	if (bActivateAbilityOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
	}
}

void UCustomGameplayAbility::OnMontageLoaded()
{
	MontageHardPtr = Montage.Get();
	if (MontageHardPtr)
	{
		K0_OnMontageLoaded(MontageHardPtr.Get());
	}
}

bool UCustomGameplayAbility::HasMontageReady() const
{
	return GetMontageToUse() != nullptr;
}

void UCustomGameplayAbility::K0_OnMontageLoaded_Implementation(UAnimMontage* LoadedMontage)
{
}

void UCustomGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	if (!ActorInfo)
	{
		return;
	}

	// Async load the montage if not available already
	if (!Montage.IsNull() && !MontageHardPtr)
	{
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			Montage.ToSoftObjectPath(),
			FStreamableDelegate::CreateWeakLambda(this, [this]()
			{
				if (!this)
				{
					return;
				}
				MontageHardPtr = this->Montage.Get();
				UE_LOG(LogTemp, Log, TEXT("%s = %d"), *this->Montage.GetAssetName(), this->Montage.IsValid());
			}), 
			FStreamableManager::AsyncLoadHighPriority
		);
		
		//Streamable.RequestAsyncLoad(Montage,FStreamableDelegate::CreateUObject(this, &ThisClass::OnMontageLoaded), FStreamableManager::AsyncLoadHighPriority);
	}
	
	K0_OnGiveAbility(*ActorInfo, Spec);
}

void UCustomGameplayAbility::K0_OnGiveAbility_Implementation(const FGameplayAbilityActorInfo& ActorInfo,
	const FGameplayAbilitySpec& Spec)
{
}

void UCustomGameplayAbility::SetupEnhancedInputBindings(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	// Check to see if the "Activation Input Action" is valid.
	UCustomGameplayAbility* const AbilityInstance = Cast<UCustomGameplayAbility>(Spec.Ability.Get());
	if (!AbilityInstance || !AbilityInstance->ActivationInputAction || (InputPressedTriggerType == ETriggerEvent::None && InputReleasedTriggerType == ETriggerEvent::None))
	{
		return;
	}
	
	const APawn* const AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
	if (!AvatarPawn || !Spec.Ability)
	{
		return;
	}
	
	const AController* const PawnController = AvatarPawn->GetController();
	UEnhancedInputComponent* const EnhancedInputComponent = PawnController ? Cast<UEnhancedInputComponent>(PawnController->InputComponent.Get()) : nullptr;
	if (!EnhancedInputComponent)
	{
		return;
	}
	
	// If we have a valid "Input Pressed Trigger" type bind the pressed event.
	if (InputPressedTriggerType != ETriggerEvent::None)
	{
		EnhancedInputComponent->BindAction(
			AbilityInstance->ActivationInputAction,
			AbilityInstance->InputPressedTriggerType,
			AbilityInstance,
			&ThisClass::HandleInputPressedEvent,
			ActorInfo,
			Spec.Handle
		);
	}

	// If we have a valid "Input Released Trigger" type bind the released event.
	if (InputReleasedTriggerType != ETriggerEvent::None)
	{
		EnhancedInputComponent->BindAction(
			AbilityInstance->ActivationInputAction,
			AbilityInstance->InputReleasedTriggerType,
			AbilityInstance,
			&ThisClass::HandleInputReleasedEvent,
			ActorInfo,
			Spec.Handle
		);
	}
}

void UCustomGameplayAbility::HandleInputPressedEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle)
{
	// Find the Ability Spec based on the passed in information and set a reference.
	FGameplayAbilitySpec* const Spec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(SpecHandle);
	if (!Spec || !Spec->Ability || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return;
	}

	UGameplayAbility* const PrimaryInstance = Spec->GetPrimaryInstance();

	UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
	if (!PrimaryInstance || !AbilitySystemComponent)
	{
		return;
	}
	
	Spec->InputPressed = true;
	
	if (!Spec->IsActive())
	{
		// Ability is not active, so try to activate it
		AbilitySystemComponent->TryActivateAbility(SpecHandle);
		return;
	}
		
	if (PrimaryInstance->bReplicateInputDirectly && !AbilitySystemComponent->IsOwnerActorAuthoritative())
	{
		AbilitySystemComponent->ServerSetInputPressed(PrimaryInstance->GetCurrentAbilitySpecHandle());
	}

	AbilitySystemComponent->AbilitySpecInputPressed(*Spec);

	// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
	AbilitySystemComponent->InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec->Handle, PrimaryInstance->GetCurrentActivationInfo().GetActivationPredictionKey());
}

void UCustomGameplayAbility::HandleInputReleasedEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle)
{
	// Find the Ability Spec based on the passed in information and set a reference.
	FGameplayAbilitySpec* const Spec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(SpecHandle);
	if (!Spec || !Spec->IsActive() || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return;
	}

	UGameplayAbility* const PrimaryInstance = Spec->GetPrimaryInstance();

	UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
	if (!PrimaryInstance || !AbilitySystemComponent)
	{
		return;
	}
	
	Spec->InputPressed = false;
	
	if (PrimaryInstance->bReplicateInputDirectly && !AbilitySystemComponent->IsOwnerActorAuthoritative())
	{
		AbilitySystemComponent->ServerSetInputReleased(SpecHandle);
	}

	AbilitySystemComponent->AbilitySpecInputReleased(*Spec);

	// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
	AbilitySystemComponent->InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, SpecHandle, PrimaryInstance->GetCurrentActivationInfoRef().GetActivationPredictionKey());
}

void UCustomGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	const APawn* const AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
	if (!AvatarPawn || !Spec.Ability)
	{
		return;
	}
	
	if (const AController* const PawnController = AvatarPawn->GetController())
	{
		if (UEnhancedInputComponent* const EnhancedInputComponent = Cast<UEnhancedInputComponent>(PawnController->InputComponent.Get()))
		{
			// We need to clear the bindings from the Enhanced Input Component when the Ability is removed.
			EnhancedInputComponent->ClearBindingsForObject(Spec.Ability.Get());
		}
	}
	
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UCustomGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	FGameplayEventData EventDataEmpty{};
	OnPreMontage(TriggerEventData != nullptr ? *TriggerEventData : EventDataEmpty);
	
	if (TriggerEventData && bHasBlueprintActivateFromEvent)
	{
		// A Blueprinted ActivateAbility function must call CommitAbility somewhere in its execution chain.
		K2_ActivateAbilityFromEvent(*TriggerEventData);
	}
	else if (bHasBlueprintActivate)
	{
		// A Blueprinted ActivateAbility function must call CommitAbility somewhere in its execution chain.
		K2_ActivateAbility();
	}
	else if (bHasBlueprintActivateFromEvent)
	{
		UE_LOG(LogAbilitySystem, Warning, TEXT("Ability %s expects event data but none is being supplied. Use 'Activate Ability' instead of 'Activate Ability From Event' in the Blueprint."), *GetName());
		constexpr bool bReplicateEndAbility = false;
		constexpr bool bWasCancelled = true;
		EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}
	else
	{
		// Native child classes should override ActivateAbility and call CommitAbility.
		// CommitAbility is used to do one last check for spending resources.
		// Previous versions of this function called CommitAbility but that prevents the callers
		// from knowing the result. Your override should call it and check the result.
		// Here is some starter code:
		
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{			
			constexpr bool bReplicateEndAbility = true;
			constexpr bool bWasCancelled = true;
			EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
			return;
		}
	}

	if (!bStartMontageOnActivation || !HasMontageReady())
	{
		return;
	}

	UAnimInstance* const AnimInstance = ActorInfo->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	PlayMontage(Handle, ActorInfo, ActivationInfo, AnimInstance);
}

void UCustomGameplayAbility::StopMontage()
{
	ACharacter* const Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (Character && (Character->GetLocalRole() == ROLE_Authority ||
						(Character->GetLocalRole() == ROLE_AutonomousProxy && GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
	{
		Character->SetAnimRootMotionTranslationScale(1.f);
	}
	
	// Check if the montage is still playing
	// The ability would have been interrupted, in which case we should automatically stop the montage

	UAnimInstance* const AnimInstance = GetCurrentActorInfo() ? GetCurrentActorInfo()->GetAnimInstance() : nullptr;
	if (AnimInstance == nullptr)
	{
		return;
	}
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	UAnimMontage* const MontageToUse = GetMontageToUse();
	if (!MontageToUse)
	{
		return;
	}
	
	if (ASC->IsAnimatingAbility(this) && ASC->GetCurrentMontage() == MontageToUse)
	{
		// Unbind delegates so they don't get called as well
		if (FAnimMontageInstance* const MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToUse))
		{
			MontageInstance->OnMontageBlendedInEnded.Unbind();
			MontageInstance->OnMontageBlendingOutStarted.Unbind();
			MontageInstance->OnMontageEnded.Unbind();
		}

		ASC->CurrentMontageStop();
	}
}

void UCustomGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bStopMontageWhenAbilityEnds)
	{
		StopMontage();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCustomGameplayAbility::OnPreMontage_Implementation(const FGameplayEventData& TriggerEventData)
{
}

void UCustomGameplayAbility::OnMontageStarted_Implementation()
{
}

void UCustomGameplayAbility::K0_OnRemoveAbility_Implementation(const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilitySpec& Spec)
{
	K0_OnRemoveAbility(ActorInfo, Spec);
}
#undef LOCTEXT_NAMESPACE