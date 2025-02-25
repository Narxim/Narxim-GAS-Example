// Copyright 2021 Joseph "Narxim" Thigpen.


#include "TurnBasedGameplayEffectComponent.h"
#include "AbilitySystemComponent.h"
#include "TurnBasedGameplayEffect.h"
#include "GameFramework/GameStateBase.h"
#include "GAS_Example/AbilitySystem/FunctionLibrary/TurnBasedAbilitySystemBlueprintLibrary.h"
#include "GAS_Example/Game/TurnBased/TurnExampleGameStateBase.h"
#include "GAS_Example/Game/TurnBased/TurnSystemInterface.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "TurnBasedGameplayEffectComponent"

DEFINE_LOG_CATEGORY(LogTurnSystem);


bool FCustomContextData_TurnBaseEffect::IsValid() const
{
	return StartTurn >= 0 && InhibitionDelayTurns >= 0 && DurationTurns > 0 && MaxPeriodicApplications >= 0;
}

FInt32Interval FCustomContextData_TurnBaseEffect::GetActivationBounds() const
{
	return FInt32Interval{GetActivationStartTurn(), GetActivationEndTurn()};
}

FInt32Interval FCustomContextData_TurnBaseEffect::GetInhibitionBounds() const
{
	return FInt32Interval{StartTurn, FMath::Max(StartTurn+InhibitionDelayTurns-1, 0)};
}

bool FCustomContextData_TurnBaseEffect::WasInhibited(const int32 CurrentTurn) const
{
	const int32 Current = GetRemainingInhibitionTurn(CurrentTurn);
	return Current == 0;
}

int32 FCustomContextData_TurnBaseEffect::GetRemainingInhibitionTurn(const int32 CurrentTurn) const
{
	auto Bounds = GetInhibitionBounds();
	return Bounds.Max - CurrentTurn + 1;
}

int32 FCustomContextData_TurnBaseEffect::GetActivationStartTurn() const
{
	return StartTurn + InhibitionDelayTurns;
}

int32 FCustomContextData_TurnBaseEffect::GetActivationEndTurn() const
{
	return GetActivationStartTurn() + DurationTurns - 1;
}

int32 FCustomContextData_TurnBaseEffect::GetRemainingActivationTurn(const int32 CurrentTurn) const
{
	if (ShouldBeInhibited(CurrentTurn))
	{
		return DurationTurns;
	}
		
	FInt32Interval Bounds = GetActivationBounds();
	if (!Bounds.Contains(CurrentTurn))
	{
		return DurationTurns;
	}

	return FMath::Max(Bounds.Max - CurrentTurn + 1, 0);
}

bool FCustomContextData_TurnBaseEffect::ShouldBeInhibited(const int32 CurrentTurn) const
{
	if (StartTurn > CurrentTurn)
	{
		return true;
	}
		
	if (InhibitionDelayTurns <= 0)
	{
		return false;
	}
	auto Bounds = GetInhibitionBounds();
	
	return Bounds.Contains(CurrentTurn);
}

bool FCustomContextData_TurnBaseEffect::ShouldBeRemoved(const int32 CurrentTurn) const
{
	return CurrentTurn > GetActivationEndTurn();
}

bool FCustomContextData_TurnBaseEffect::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bool bCombinedSuccess = FCustomContextDataBase::NetSerialize(Ar, Map, bOutSuccess);

	enum RepFlag
	{
		REP_StartTurn,
		REP_InhibitionDelayTurns,
		REP_DurationTurns,
		REP_MaxPeriodicApplications,
		REP_MAX
	};

	uint16 RepBits = 0;
	if (Ar.IsSaving())
	{
		if (StartTurn > 0)
		{
			RepBits |= 1 << REP_StartTurn;
		}
		if (InhibitionDelayTurns > 0)
		{
			RepBits |= 1 << REP_InhibitionDelayTurns;
		}
		if (DurationTurns > 0)
		{
			RepBits |= 1 << REP_DurationTurns;
		}
		if (StartTurn > 0)
		{
			RepBits |= 1 << REP_MaxPeriodicApplications;
		}
	}

	Ar.SerializeBits(&RepBits, REP_MAX);
	if (RepBits & (1 << REP_StartTurn))
	{
		Ar << StartTurn;
	}
	if (RepBits & (1 << REP_InhibitionDelayTurns))
	{
		Ar << InhibitionDelayTurns;
	}
	if (RepBits & (1 << REP_DurationTurns))
	{
		Ar << DurationTurns;
	}
	if (RepBits & (1 << REP_MaxPeriodicApplications))
	{
		Ar << MaxPeriodicApplications;
	}

	return bCombinedSuccess;
}

UTurnBasedGameplayEffectComponent::UTurnBasedGameplayEffectComponent()
{
#if WITH_EDITORONLY_DATA
	EditorFriendlyName = TEXT("Turn Based Support (behave based on turns)");
#endif // WITH_EDITORONLY_DATA
}

#if WITH_EDITOR
EDataValidationResult UTurnBasedGameplayEffectComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	// Invalid duration type (Instant)
	if (GetOwner()->DurationPolicy == EGameplayEffectDurationType::Instant)
	{
	    Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidInstantDurationError",
	    	"[{0}] Requires Infinite Duration \n"
			"- This component only works with Gameplay Effects using 'Infinite' duration policy"),
	    	FText::FromString(GetClass()->GetName())));
	    Result = EDataValidationResult::Invalid;
	}

	// Invalid duration type (HasDuration)
	if (GetOwner()->DurationPolicy == EGameplayEffectDurationType::HasDuration)
	{
	    Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidHasDurationError",
	        "[{0}] Requires Infinite Duration - Component is incompatible with 'Has Duration' policy.\n"
			"Change parent GE to use 'Infinite'"),
	        FText::FromString(GetClass()->GetName())));
	    Result = EDataValidationResult::Invalid;
	}

	// Invalid Uninhibition GE duration
	for (const TSubclassOf<UGameplayEffect>& Effect : EffectToApplyOnUninhibition)
	{
		if (IsValid(Effect))
		{
			const UGameplayEffect* const UninhibitionGE = Effect->GetDefaultObject<UGameplayEffect>();
			check(UninhibitionGE != nullptr);
			if (UninhibitionGE->DurationPolicy == EGameplayEffectDurationType::Infinite)
			{
				Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidUninhibitionGEDurationError",
					"[{0}] Invalid 'On Uninhibition Effect'\n"
					"- Uses Infinite duration (will never be removed). Use Instant or HasDuration"),
					FText::FromString(GetClass()->GetName())));
				Result = EDataValidationResult::Invalid;
			}
		}
	}

	// Invalid Removal GE duration
	for (const TSubclassOf<UGameplayEffect>& Effect : EffectToApplyOnRemoval)
	{
		if (IsValid(Effect))
		{
			const UGameplayEffect* const RemovalGE = Effect->GetDefaultObject<UGameplayEffect>();
			check(RemovalGE != nullptr);
			if (RemovalGE->DurationPolicy == EGameplayEffectDurationType::Infinite)
			{
				Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidRemovalGEDurationError",
					"[{0}] Invalid 'On Removal Effect' - Infinite duration would prevent automatic removal"),
					FText::FromString(GetClass()->GetName())));
				Result = EDataValidationResult::Invalid;
			}
		}
	}
	

	// Invalid Periodic GE duration
	for (const TSubclassOf<UGameplayEffect>& Effect : EffectToApplyPeriodically)
	{
		if (IsValid(Effect))
		{
			const UGameplayEffect* const PeriodicGE = Effect->GetDefaultObject<UGameplayEffect>();
			check(PeriodicGE != nullptr);
			if (PeriodicGE->DurationPolicy == EGameplayEffectDurationType::Infinite)
			{
				Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidPeriodicGEDurationError",
					"[{0}] Invalid 'Periodic Effect' - Must use Instant/HasDuration policy to ensure removal"),
					FText::FromString(GetClass()->GetName())));
				Result = EDataValidationResult::Invalid;
			}
		}
	}

	// Negative Inhibition Delay
	if (InhibitionDelayTurns.GetValueAtLevel(0) < 0)
	{
	    Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidInhibitionDelayError",
	        "[{0}] Invalid 'Inhibition Delay' value: {1}\n"
	        "Must be ≥ 0. Use 0 for immediate activation"),
	        FText::FromString(GetClass()->GetName()),
	        FText::AsNumber(InhibitionDelayTurns.GetValueAtLevel(0))));
	    Result = EDataValidationResult::Invalid;
	}

	// Negative Duration
	if (DurationTurns.GetValueAtLevel(0) < 0)
	{
	    Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidDurationError",
	        "[{0}] Invalid 'Duration Turns' value: {1}\n"
	        "Must be ≥ 0. Use 0 for instant removal after activation"),
	        FText::FromString(GetClass()->GetName()),
	        FText::AsNumber(DurationTurns.GetValueAtLevel(0))));
	    Result = EDataValidationResult::Invalid;
	}

	// Invalid MaxApplications value
	if (MaxPeriodicApplications.GetValueAtLevel(0) < 1)
	{
		Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidMaxApplicationsError",
			"[{0}] 'Max Applications' must be at least 1 when limiting periodic effects"),
			FText::FromString(GetClass()->GetName())));
		Result = EDataValidationResult::Invalid;
	}

	// Max exceeds duration
	if (bLimitPeriodicApplications && MaxPeriodicApplications.GetValueAtLevel(0) > DurationTurns.GetValueAtLevel(0))
	{
		Context.AddWarning(FText::FormatOrdered(LOCTEXT("MaxExceedsDurationWarning",
			"[{0}] Conflict: 'Max Applications' ({1}) > 'Duration Turns' ({2})\n"
			"Periodic effect will only apply {2} times before removal"),
			FText::FromString(GetClass()->GetName()),
			FText::AsNumber(MaxPeriodicApplications.GetValueAtLevel(0)),
			FText::AsNumber(DurationTurns.GetValueAtLevel(0))));
		Result = EDataValidationResult::Invalid;
	}

	// Missing periodic GE
	if (bEnablePeriodicGE)
	{
		for (const TSubclassOf<UGameplayEffect>& Effect : EffectToApplyPeriodically)
		{
			if (IsValid(Effect))
			{
				continue;
			}
			Context.AddError(FText::FormatOrdered(LOCTEXT("MissingPeriodicGEError",
				"[{0}] Periodic effect enabled but no 'Periodic Effect' assigned"),
				FText::FromString(GetClass()->GetName())));
			Result = EDataValidationResult::Invalid;
		}
	}

	// Zero duration with periodic
	if (bEnablePeriodicGE && DurationTurns == 0)
	{
		Context.AddError(FText::FormatOrdered(LOCTEXT("ZeroDurationPeriodicError",
			"[{0}] Contradiction: 'Duration Turns' is 0 but 'Periodic Effect' is enabled\n"
			"Periodic effects need at least 1 turn duration to apply"),
			FText::FromString(GetClass()->GetName())));
		Result = EDataValidationResult::Invalid;
	}
	 	
	return Result;
}

void UTurnBasedGameplayEffectComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// // Automatically fix common mistakes
	// InhibitionDelayTurns = FMath::Max(0, InhibitionDelayTurns.GetValueAtLevel(0));
	// DurationTurns = FMath::Max(0, DurationTurns.GetValueAtLevel(0));
}
#endif // WITH_EDITOR

bool UTurnBasedGameplayEffectComponent::CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec) const
{
	if (!Super::CanGameplayEffectApply(ActiveGEContainer, GESpec) || !UTurnBasedAbilitySystemBlueprintLibrary::GetTurnSystem(ActiveGEContainer.Owner))
	{
		return false;
	}
	
	FCustomContextData_TurnBaseEffect Data = UTurnBasedAbilitySystemBlueprintLibrary::GetTurnBasedEffectInstanceDataFromSpec(GESpec);
	return Data.IsValid();
}

bool UTurnBasedGameplayEffectComponent::OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer, FActiveGameplayEffect& ActiveGE) const
{
	// Needs to be done on client too, to register the various listeners.
	UAbilitySystemComponent* const OwnerASC = ActiveGEContainer.Owner;

	// Early exit if OwnerASC is invalid. Use IsValid() for UObjects.
	if (!IsValid(OwnerASC))
	{
		UE_LOG(LogTurnSystem, Warning, TEXT("OnActiveGameplayEffectAdded: OwnerASC is invalid."));
		return false; // Inhibit the effect
	}

	ITurnSystemInterface* const TurnSystem = UTurnBasedAbilitySystemBlueprintLibrary::GetTurnSystem(OwnerASC);

	// Early exit if the subsystem is invalid.
	if (TurnSystem == nullptr)
	{
		UE_LOG(LogTurnSystem, Warning, TEXT("OnActiveGameplayEffectAdded: TurnSystem is invalid."));
		return false; // Inhibit the effect
	}

	FCustomContextData_TurnBaseEffect Data = UTurnBasedAbilitySystemBlueprintLibrary::GetTurnBasedEffectInstanceDataFromSpec(ActiveGE.Spec);

	const int32 CurrentTurn = TurnSystem->GetCurrentTurn();

	// Add the OnTurnChange event. Store the handle.
	FDelegateHandle DelegateHandle = TurnSystem->GetOnTurnChangeDelegate().AddUObject(
		this,
		&UTurnBasedGameplayEffectComponent::OnTurnChange,
		Data.StartTurn,
		ActiveGE.Handle
	);

	// Handle potential delegate add failure
	if (!DelegateHandle.IsValid())
	{
		UE_LOG(LogTurnSystem, Warning, TEXT("OnActiveGameplayEffectAdded: Failed to add OnTurnChange delegate."));
		return false; // Inhibit the effect
	}

	// Store the delegate handle in the ActiveGE for later removal.  Using a custom struct or map in the ActiveGE is recommended for more complex scenarios.
	ActiveGE.EventSet.OnEffectRemoved.AddUObject(
		this,
		&UTurnBasedGameplayEffectComponent::OnGameplayEffectRemoved,
		OwnerASC,
		DelegateHandle);

	// Check if the OnEffectRemoved delegate was successfully bound.
	if (!ActiveGE.EventSet.OnEffectRemoved.IsBound())
	{
		UE_LOG(LogTurnSystem, Error, TEXT("OnActiveGameplayEffectAdded: Failed to add OnEffectRemoved delegate."));
		TurnSystem->GetOnTurnChangeDelegate().Remove(DelegateHandle); // Clean up the OnTurnChange delegate.
		return false;
	}

	// Allow the effect to be added uninhibited
	bool bApplyUninhibited = !Data.ShouldBeInhibited(CurrentTurn);

	// Don't process the next code: authoritative only.
	if (!IsValid(ActiveGEContainer.Owner) || !ActiveGEContainer.Owner->IsOwnerActorAuthoritative())
	{
		return bApplyUninhibited;
	}

	if (bApplyUninhibited && OwnerASC->IsOwnerActorAuthoritative())
	{
		for (const TSubclassOf<UGameplayEffect>& Effect : EffectToApplyOnUninhibition)
		{
			if (IsValid(Effect))
			{
				UTurnBasedAbilitySystemBlueprintLibrary::ApplyTurnBasedGameEffect(OwnerASC, ActiveGE, Effect, bCopyContextFromSource);
			}
		}

		// Check if we should be removed, still. Shouldn't happen though, duration >= 1
		if (Data.ShouldBeRemoved(CurrentTurn))
		{
			OwnerASC->RemoveActiveGameplayEffect(ActiveGE.Handle);
		}
	}

	return bApplyUninhibited; 
}

void UTurnBasedGameplayEffectComponent::OnTurnChange(int32 NewTurn, int32 StartTurn, FActiveGameplayEffectHandle Handle) const
{
	UAbilitySystemComponent* const OwnerASC = Handle.GetOwningAbilitySystemComponent();
	if (!IsValid(OwnerASC))
	{
		UE_LOG(LogTemp, Warning, TEXT("OnTurnChange: Invalid OwnerASC for handle %s"), *Handle.ToString());
		return;
	}

	const FActiveGameplayEffect* const ActiveEffect = OwnerASC->GetActiveGameplayEffect(Handle);
	if (!ActiveEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnTurnChange: Invalid ActiveEffect for handle %s"), *Handle.ToString());
		return;
	}

	FCustomContextData_TurnBaseEffect Data = UTurnBasedAbilitySystemBlueprintLibrary::GetTurnBasedEffectInstanceDataFromSpec(ActiveEffect->Spec);
	if (!Data.IsValid())
	{
		OwnerASC->RemoveActiveGameplayEffect(Handle);
		return;
	}

	if (Data.ShouldBeInhibited(NewTurn))
	{
		return;
	}

	// Never true on the first turn as you still want to see the effect.
	bool bShouldRemove = Data.ShouldBeRemoved(NewTurn);
	
	FInt32Interval ActivationBounds = Data.GetActivationBounds();

	const bool bWasInhibited = Data.WasInhibited(NewTurn);
	if (bWasInhibited)
	{
		// Unhibit the effect if it has started inhibited
		OwnerASC->SetActiveGameplayEffectInhibit(MoveTemp(Handle), false, true);

		if (OwnerASC->IsOwnerActorAuthoritative())
		{
			for (const TSubclassOf<UGameplayEffect>& Effect : EffectToApplyOnUninhibition)
			{
				if (!IsValid(Effect))
				{
					continue;
				}
				UTurnBasedAbilitySystemBlueprintLibrary::ApplyTurnBasedGameEffect(OwnerASC, *ActiveEffect, Effect, bCopyContextFromSource);
			}
		}
	}

	if (!bWasInhibited)
	{
		ProcessPeriodicEffect(OwnerASC, ActivationBounds.Max - NewTurn, Handle);
	}

	if (!bShouldRemove)
	{
		return;
	}

	OwnerASC->RemoveActiveGameplayEffect(Handle);
}

void UTurnBasedGameplayEffectComponent::OnGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, UAbilitySystemComponent* ASC, FDelegateHandle DelegateHandle) const
{
	if (!IsValid(ASC))
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted GE removal without valid ASC %s"), IsValid(ASC) ? *ASC->GetName() : TEXT("NULL"));
		return;
	}
	
	if (!DelegateHandle.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid DelegateHandle on ASC %s"), *ASC->GetName());
		return;
	}

	// Don't forget to remove the callback from the 'global' OnTurnChange delegate
	if (ITurnSystemInterface* const TurnSystem = UTurnBasedAbilitySystemBlueprintLibrary::GetTurnSystem(ASC))
	{
		TurnSystem->GetOnTurnChangeDelegate().Remove(DelegateHandle);
	}

	if (!ASC->IsOwnerActorAuthoritative() || !RemovalInfo.ActiveEffect)
	{
		return;
	}

	// Only authority by this point.
	
	for (const TSubclassOf<UGameplayEffect>& Effect : EffectToApplyOnRemoval)
	{
		if (!IsValid(Effect))
		{
			continue;
		}
		UTurnBasedAbilitySystemBlueprintLibrary::ApplyTurnBasedGameEffect(ASC, *RemovalInfo.ActiveEffect, Effect, bCopyContextFromSource);
	}
}

void UTurnBasedGameplayEffectComponent::ProcessPeriodicEffect(UAbilitySystemComponent* OwnerASC, const int32 ActiveTurns, FActiveGameplayEffectHandle Handle) const
{
	if (!bEnablePeriodicGE || EffectToApplyPeriodically.IsEmpty())
	{
		return;
	}
	
	if (!OwnerASC || !Handle.IsValid() || !OwnerASC->IsOwnerActorAuthoritative())
	{
		return;
	}
	
	const FActiveGameplayEffect* const ActiveEffect = OwnerASC->GetActiveGameplayEffect(Handle);
	if (!ActiveEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProcessPeriodicEffect: Invalid ActiveEffect for handle %s"), *Handle.ToString());
		return;
	}

	FCustomContextData_TurnBaseEffect Data = UTurnBasedAbilitySystemBlueprintLibrary::GetTurnBasedEffectInstanceDataFromSpec(ActiveEffect->Spec);

	bool bShouldApply = true;
	if (bLimitPeriodicApplications)
	{
		// Ensure we don't exceed the max applications
		bShouldApply = ActiveTurns < Data.MaxPeriodicApplications;
	}

	if (!bShouldApply)
	{
		return;
	}

	for (const TSubclassOf<UGameplayEffect>& Effect : EffectToApplyPeriodically)
	{
		if (!IsValid(Effect))
		{
			continue;
		}
		UTurnBasedAbilitySystemBlueprintLibrary::ApplyTurnBasedGameEffect(OwnerASC, *ActiveEffect, Effect, bCopyContextFromSource);
	}
}

#undef LOCTEXT_NAMESPACE