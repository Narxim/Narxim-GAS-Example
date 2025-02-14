// Copyright 2021 Joseph "Narxim" Thigpen.


#include "TurnBasedGameplayEffectComponent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GAS_Example/Game/TurnSystemInterface.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "TurnBasedGameplayEffectComponent"

DEFINE_LOG_CATEGORY(LogTurnSystem);

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
	if (IsValid(GEToApplyOnUninhibition))
	{
	    const UGameplayEffect* DefaultGE = GEToApplyOnUninhibition->GetDefaultObject<UGameplayEffect>();
	    check(DefaultGE != nullptr);
	    if (DefaultGE->DurationPolicy == EGameplayEffectDurationType::Infinite)
	    {
	        Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidUninhibitionGEDurationError",
	            "[{0}] Invalid 'On Uninhibition Effect'\n"
	            "- Uses Infinite duration (will never be removed). Use Instant or HasDuration"),
	            FText::FromString(GetClass()->GetName())));
	        Result = EDataValidationResult::Invalid;
	    }
	}

	// Invalid Removal GE duration
	if (IsValid(GEToApplyOnRemoval))
	{
	    const UGameplayEffect* DefaultGE = GEToApplyOnRemoval->GetDefaultObject<UGameplayEffect>();
	    check(DefaultGE != nullptr);
	    if (DefaultGE->DurationPolicy == EGameplayEffectDurationType::Infinite)
	    {
	        Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidRemovalGEDurationError",
	        	"[{0}] Invalid 'On Removal Effect' - Infinite duration would prevent automatic removal"),
	        	FText::FromString(GetClass()->GetName())));
	        Result = EDataValidationResult::Invalid;
	    }
	}

	// Invalid Periodic GE duration
	if (IsValid(GEToApplyPeriodically))
	{
	    const UGameplayEffect* DefaultGE = GEToApplyPeriodically->GetDefaultObject<UGameplayEffect>();
	    check(DefaultGE != nullptr);
	    if (DefaultGE->DurationPolicy == EGameplayEffectDurationType::Infinite)
	    {
	        Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidPeriodicGEDurationError",
	            "[{0}] Invalid 'Periodic Effect' - Must use Instant/HasDuration policy to ensure removal"),
	            FText::FromString(GetClass()->GetName())));
	        Result = EDataValidationResult::Invalid;
	    }
	}

	// Negative Inhibition Delay
	if (InhibitionDelayTurns < 0)
	{
	    Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidInhibitionDelayError",
	        "[{0}] Invalid 'Inhibition Delay' value: {1}\n"
	        "Must be ≥ 0. Use 0 for immediate activation"),
	        FText::FromString(GetClass()->GetName()),
	        FText::AsNumber(InhibitionDelayTurns)));
	    Result = EDataValidationResult::Invalid;
	}

	// Negative Duration
	if (DurationTurns < 0)
	{
	    Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidDurationError",
	        "[{0}] Invalid 'Duration Turns' value: {1}\n"
	        "Must be ≥ 0. Use 0 for instant removal after activation"),
	        FText::FromString(GetClass()->GetName()),
	        FText::AsNumber(DurationTurns)));
	    Result = EDataValidationResult::Invalid;
	}

	// Invalid MaxApplications value
	if (MaxPeriodicApplications < 1)
	{
		Context.AddError(FText::FormatOrdered(LOCTEXT("InvalidMaxApplicationsError",
			"[{0}] 'Max Applications' must be at least 1 when limiting periodic effects"),
			FText::FromString(GetClass()->GetName())));
		Result = EDataValidationResult::Invalid;
	}

	// Max exceeds duration
	if (bLimitPeriodicApplications && MaxPeriodicApplications > DurationTurns)
	{
		Context.AddWarning(FText::FormatOrdered(LOCTEXT("MaxExceedsDurationWarning",
			"[{0}] Conflict: 'Max Applications' ({1}) > 'Duration Turns' ({2})\n"
			"Periodic effect will only apply {2} times before removal"),
			FText::FromString(GetClass()->GetName()),
			FText::AsNumber(MaxPeriodicApplications),
			FText::AsNumber(DurationTurns)));
		Result = EDataValidationResult::Invalid;
	}

	// Missing periodic GE
	if (bEnablePeriodicGE && !IsValid(GEToApplyPeriodically))
	{
		Context.AddError(FText::FormatOrdered(LOCTEXT("MissingPeriodicGEError",
			"[{0}] Periodic effect enabled but no 'Periodic Effect' assigned"),
			FText::FromString(GetClass()->GetName())));
		Result = EDataValidationResult::Invalid;
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

	// Automatically fix common mistakes
	InhibitionDelayTurns = FMath::Max(0, InhibitionDelayTurns);
	DurationTurns = FMath::Max(0, DurationTurns);
}
#endif // WITH_EDITOR

bool UTurnBasedGameplayEffectComponent::CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec) const
{
	if (GetTurnSystem(ActiveGEContainer.Owner) != nullptr)
	{
		// For now, keeping it simple: As long as we're able to access the TurnSubsystem, then the effect can always be applied
		return true;
	}

	return false;
}

bool UTurnBasedGameplayEffectComponent::OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer, FActiveGameplayEffect& ActiveGE) const
{
    UAbilitySystemComponent* OwnerASC = ActiveGEContainer.Owner;

    // Early exit if OwnerASC is invalid. Use IsValid() for UObjects.
    if (!IsValid(OwnerASC))
    {
        UE_LOG(LogTurnSystem, Warning, TEXT("OnActiveGameplayEffectAdded: OwnerASC is invalid."));
        return false; // Inhibit the effect
    }

    ITurnSystemInterface* TurnSystem = GetTurnSystem(OwnerASC);

    // Early exit if the subsystem is invalid.
    if (TurnSystem == nullptr)
    {
        UE_LOG(LogTurnSystem, Warning, TEXT("OnActiveGameplayEffectAdded: TurnSystem is invalid."));
        return false; // Inhibit the effect
    }

    int32 StartTurn = TurnSystem->Execute_GetCurrentTurn(TurnSystem->_getUObject());

    // Add the OnTurnChange event. Store the handle.
    FDelegateHandle DelegateHandle = TurnSystem->GetOnTurnChangeDelegate().AddUObject(
        this,
        &UTurnBasedGameplayEffectComponent::OnTurnChange,
        StartTurn,
        ActiveGE.Handle);
	
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
	bool bApplyUninhibited = InhibitionDelayTurns == 0;
	if (bApplyUninhibited)
	{
		if (IsValid(GEToApplyOnUninhibition))
		{
			ApplyGameEffect(OwnerASC, GEToApplyOnUninhibition);
		}

		// Don't apply periodic effect if we're pending removal
		if (DurationTurns == 0)
		{
			OwnerASC->RemoveActiveGameplayEffect(ActiveGE.Handle);
		}
		else
		{
			ProcessPeriodicEffect(OwnerASC, 0);
		}
	}
	
	return bApplyUninhibited; 
}

void UTurnBasedGameplayEffectComponent::OnTurnChange(int32 NewTurn, int32 StartTurn, FActiveGameplayEffectHandle Handle) const
{
	UAbilitySystemComponent* OwnerASC = Handle.GetOwningAbilitySystemComponent();
	if (!IsValid(OwnerASC))
	{
		UE_LOG(LogTemp, Warning, TEXT("OnTurnChange: Invalid OwnerASC for handle %s"), 
			*Handle.ToString());
		return;
	}

	bool bShouldRemove = false;
	
	if (InhibitionDelayTurns == NewTurn - StartTurn) 
	{
		// Unhibit the effect if it has started inhibited
		OwnerASC->SetActiveGameplayEffectInhibit(MoveTemp(Handle), false, true);

		if (IsValid(GEToApplyOnUninhibition))
		{
			ApplyGameEffect(OwnerASC, GEToApplyOnUninhibition);
		}
			
		// Now that we've uninhibited, check if we should remove this effect immediately 
		bShouldRemove  = DurationTurns == 0;
	}
	
	// Calculate activeTurn after activation delay
	const int ActiveTurns = NewTurn - StartTurn - InhibitionDelayTurns;
	bShouldRemove = bShouldRemove || DurationTurns == ActiveTurns; 
	
	// Don't apply periodic effect if we're pending removal
	if (!bShouldRemove)
	{
		ProcessPeriodicEffect(OwnerASC, ActiveTurns);
	}
	
	if (bShouldRemove)
	{
		OwnerASC->RemoveActiveGameplayEffect(Handle);
	}
}

void UTurnBasedGameplayEffectComponent::OnGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, UAbilitySystemComponent* ASC, FDelegateHandle DelegateHandle) const
{
	if (!IsValid(ASC) || !ASC->IsOwnerActorAuthoritative())
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted GE removal without authority on ASC %s"), 
			IsValid(ASC) ? *ASC->GetName() : TEXT("NULL"));
		return;
	}
	
	if (!DelegateHandle.IsValid())  
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid DelegateHandle on ASC %s"), *ASC->GetName());
		return;
	}

	// Don't forget to remove the callback from the 'global' OnTurnChange delegate
	if (ITurnSystemInterface* TurnSystem = GetTurnSystem(ASC))
	{
		TurnSystem->GetOnTurnChangeDelegate().Remove(DelegateHandle);
	}

	if (IsValid(GEToApplyOnRemoval))
	{
		ApplyGameEffect(ASC, GEToApplyOnRemoval);
	}
}

void UTurnBasedGameplayEffectComponent::ApplyGameEffect(UAbilitySystemComponent* ASC, const TSubclassOf<UGameplayEffect>& GameEffectToApply)
{
	if (!IsValid(ASC) || !ASC->IsOwnerActorAuthoritative())
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyGameEffect failed on ASC %s: Invalid or non-authoritative"),
			IsValid(ASC) ? *ASC->GetName() : TEXT("NULL"));
		return;
	}
	
	if (!IsValid(GameEffectToApply))  
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid GameEffectToApply in ASC %s"), *ASC->GetName());
		return;
	}
	
	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle NewSpecHandle = ASC->MakeOutgoingSpec(GameEffectToApply, 1, EffectContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*NewSpecHandle.Data.Get());
}

ITurnSystemInterface* UTurnBasedGameplayEffectComponent::GetTurnSystem(const UAbilitySystemComponent* ASC)
{
	if (!IsValid(ASC))
	{
		UE_LOG(LogTurnSystem, Error, TEXT("Null ASC in GetTurnSubsystem"));
		return nullptr;
	}
    
	UWorld* World = ASC->GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTurnSystem, Error, TEXT("ASC %s has invalid world"), *ASC->GetName());
		return nullptr;
	}
	
	AGameStateBase* GameState = World->GetGameState();
	if (!IsValid(GameState))
	{
		UE_LOG(LogTurnSystem, Error, TEXT("World has no valid GameState"));
		return nullptr;
	}
	
	ITurnSystemInterface* FoundTurnSystem = Cast<ITurnSystemInterface>(GameState);
	if (!FoundTurnSystem)
	{
		UE_LOG(LogTurnSystem, Error, TEXT("GameState %s does not implement ITurnSystemInterface"), *GameState->GetName());
		return nullptr;
	}

	return FoundTurnSystem;
}


void UTurnBasedGameplayEffectComponent::ProcessPeriodicEffect(UAbilitySystemComponent* OwnerASC, const int ActiveTurns) const
{
	if (bEnablePeriodicGE && IsValid(GEToApplyPeriodically))
	{
		bool bShouldApply = true;
		if (bLimitPeriodicApplications)
		{
			// Ensure we don't exceed the max applications
			bShouldApply = ActiveTurns < MaxPeriodicApplications;
		}

		if (bShouldApply)
		{
			ApplyGameEffect(OwnerASC, GEToApplyPeriodically);
		}
	}
}

#undef LOCTEXT_NAMESPACE