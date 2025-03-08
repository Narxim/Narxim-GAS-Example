// Copyright 2021 Joseph "Narxim" Thigpen.


#include "TurnBasedAbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GAS_Example/AbilitySystem/GameplayEffects/TurnBased/TurnBasedGameplayEffect.h"
#include "GAS_Example/AbilitySystem/GameplayEffects/TurnBased/TurnBasedGameplayEffectComponent.h"
#include "GAS_Example/Game/TurnBased/TurnExampleGameStateBase.h"


void UTurnBasedAbilitySystemBlueprintLibrary::ApplyTurnBasedGameEffect(UAbilitySystemComponent* ASC, const FActiveGameplayEffect& ActiveGE, const TSubclassOf<UGameplayEffect>& GameEffectToApply, const bool bCopyContext /* = true */)
{
	if (!ASC || !ASC->IsOwnerActorAuthoritative())
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyGameEffect failed on ASC %s: Invalid or non-authoritative"), ASC ? *ASC->GetName() : TEXT("NULL"));
		return;
	}
	
	if (!GameEffectToApply)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid GameEffectToApply in ASC %s"), *ASC->GetName());
		return;
	}
	
	FGameplayEffectContextHandle EffectContextHandle = bCopyContext ? ActiveGE.Spec.GetContext().Duplicate() : ASC->MakeEffectContext();

	if (GameEffectToApply.GetDefaultObject()->IsA(UTurnBasedGameplayEffect::StaticClass()))
	{
		FGameplayTagContainer SourceTags = ASC->GetOwnedGameplayTags();
		SourceTags.AppendTags(GameEffectToApply.GetDefaultObject()->GetAssetTags());
		
		TSubclassOf<UTurnBasedGameplayEffect> MyTurnClass{GameEffectToApply};
		UTurnBasedGameplayEffect::UpdateTurnBasedEffectContext(EffectContextHandle, ActiveGE.Spec.GetLevel(), MyTurnClass, ASC, SourceTags);
	}
	
	FGameplayEffectSpecHandle NewSpecHandle = ASC->MakeOutgoingSpec(GameEffectToApply, ActiveGE.Spec.GetLevel(), EffectContextHandle);
	if (NewSpecHandle.IsValid())
	{
		ASC->ApplyGameplayEffectSpecToSelf(*NewSpecHandle.Data.Get());
	}
}

ATurnExampleGameStateBase* UTurnBasedAbilitySystemBlueprintLibrary::GetTurnSystem(const UObject* WorldContextObject)
{
	if (UWorld* const World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr)
	{
		return Cast<ATurnExampleGameStateBase>(World->GetGameState());
	}

	return nullptr;
}

FCustomContextData_TurnBaseEffect UTurnBasedAbilitySystemBlueprintLibrary::GetTurnBasedEffectInstanceDataFromActiveEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (!Handle.IsValid() || !Handle.GetOwningAbilitySystemComponent())
	{
		return FCustomContextData_TurnBaseEffect();
	}

	const FActiveGameplayEffect* const ActiveEffect = Handle.GetOwningAbilitySystemComponent()->GetActiveGameplayEffect(Handle);
	if (!ActiveEffect)
	{
		return FCustomContextData_TurnBaseEffect();
	}

	return GetTurnBasedEffectInstanceDataFromSpec(ActiveEffect->Spec);
}


const UTurnBasedGameplayEffectComponent* UTurnBasedAbilitySystemBlueprintLibrary::ExtractTurnBasedEffectComponentFromEffectHandle(FActiveGameplayEffectHandle ActiveHandle)
{
	if (!ActiveHandle.GetOwningAbilitySystemComponent())
	{
		return nullptr;
	}

	const UTurnBasedGameplayEffect* const TurnEffect = Cast<UTurnBasedGameplayEffect>(ActiveHandle.GetOwningAbilitySystemComponent()->GetGameplayEffectDefForHandle(ActiveHandle));
	if (!TurnEffect)
	{
		return nullptr;
	}

	return ExtractTurnBasedEffectComponentFromGE(TurnEffect);
}

const UTurnBasedGameplayEffectComponent* UTurnBasedAbilitySystemBlueprintLibrary::ExtractTurnBasedEffectComponentFromGE(const UTurnBasedGameplayEffect* TurnEffect)
{
	if (!TurnEffect)
	{
		return nullptr;
	}

	return Cast<UTurnBasedGameplayEffectComponent>(TurnEffect->FindComponent(UTurnBasedGameplayEffectComponent::StaticClass()));
}

FCustomContextData_TurnBaseEffect UTurnBasedAbilitySystemBlueprintLibrary::GetUpdatedTurnDataBasedOnTurn(const UObject* WorldContextObject, const FCustomContextData_TurnBaseEffect& TurnBaseData, const int32 TurnOverride /* = -1 */)
{
	int32 CurrentTurn = TurnOverride;
	if (CurrentTurn < 0)
	{
		ITurnSystemInterface* const TurnSystem = GetTurnSystem(WorldContextObject);

		// Early exit if the subsystem is invalid.
		if (TurnSystem == nullptr)
		{
			UE_LOG(LogTurnSystem, Warning, TEXT("UpdateTurnData: TurnSystem is invalid."));
			return FCustomContextData_TurnBaseEffect();
		}
		CurrentTurn = TurnSystem->GetCurrentTurn();
	}

	FCustomContextData_TurnBaseEffect Updated = TurnBaseData;

	Updated.StartTurn = CurrentTurn;
	Updated.InhibitionDelayTurns = TurnBaseData.GetRemainingInhibitionTurn(CurrentTurn);
	Updated.DurationTurns = TurnBaseData.GetRemainingActivationTurn(CurrentTurn);

	return Updated;
}

FCustomContextData_TurnBaseEffect UTurnBasedAbilitySystemBlueprintLibrary::GetTurnBasedEffectInstanceDataFromSpec(const FGameplayEffectSpec& Spec)
{
	const FCustomGameplayEffectContext* const EffectContext = static_cast<const FCustomGameplayEffectContext*>(Spec.GetContext().Get());
	if (!EffectContext)
	{
		return FCustomContextData_TurnBaseEffect();
	}
	
	for (const auto& ContextData : EffectContext->GetAllCustomContextData())
	{
		if (ContextData.GetScriptStruct() != FCustomContextData_TurnBaseEffect::StaticStruct())
		{
			continue;
		}
		
		if (const FCustomContextData_TurnBaseEffect* TurnBaseEffectData = ContextData.GetPtr<FCustomContextData_TurnBaseEffect>())
		{
			return *TurnBaseEffectData;
		}
	}

	return FCustomContextData_TurnBaseEffect();
}
