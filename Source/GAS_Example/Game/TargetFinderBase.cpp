// Copyright 2021 Joseph "Narxim" Thigpen.
#include "TargetFinderBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

void UTargetFinderBase::K0_FindTargets_Implementation(const AActor* SourceActor, TArray<AActor*>& OutActors, TArray<FHitResult>& OutHitResults)
{
}

bool UTargetFinderBase::CheckTagRequirementsForTarget(const AActor* Target)
{
	if (!IsValid(Target))
	{
		return false;
	}

	if (TagRequirements.IsEmpty())
	{
		return true;
	}

	const UAbilitySystemComponent* const ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
	if (!ASC)
	{
		return false;
	}

	FGameplayTagContainer OwnedTags;
	ASC->GetOwnedGameplayTags(OwnedTags);

	return TagRequirements.RequirementsMet(OwnedTags);
}

void UTargetFinderBase::TraceTargets_Implementation(const AActor* SourceActor, TArray<AActor*>& OutActors, TArray<FHitResult>& OutHitResults)
{
}

bool UTargetFinderBase::IsTargetValid(const AActor* SourceActor, const AActor* TargetActor)
{
	if (!CheckTagRequirementsForTarget(TargetActor))
	{
		return false;
	}
	
	return K0_IsTargetValid(SourceActor, TargetActor);
}

bool UTargetFinderBase::K0_IsTargetValid_Implementation(const AActor* SourceActor,const AActor* TargetActor)
{
	return true;
}

void UTargetFinderBase::FindTargets(const AActor* SourceActor, TArray<AActor*>& OutActors, TArray<FHitResult>& OutHitResults, ETargetFinder_Pins& Pins)
{
	{
		auto ImplementedInBlueprint = [](const UFunction* Func) -> bool
		{
			return Func && ensure(Func->GetOuter())
				&& Func->GetOuter()->IsA(UBlueprintGeneratedClass::StaticClass());
		};
		
		static FName FuncName = FName(TEXT("K0_FindTargets"));
		UFunction* FindTargetBP = GetClass()->FindFunctionByName(FuncName);
		if (ImplementedInBlueprint(FindTargetBP))
		{
			K0_FindTargets(SourceActor, OutActors, OutHitResults);
			return;
		}
	}

	if (!IsValid(SourceActor))
	{
		return;
	}

	TraceTargets(SourceActor , OutActors, OutHitResults);
	
	if (!OutActors.IsEmpty())
	{
		for (auto TargetActor = OutActors.CreateIterator(); TargetActor; ++TargetActor)
		{
			if (!CheckTagRequirementsForTarget(*TargetActor))
			{
				TargetActor.RemoveCurrent();
			}
		}
	}

	if (!OutHitResults.IsEmpty())
	{
		for (auto TargetHit = OutHitResults.CreateIterator(); TargetHit; ++TargetHit)
		{
			const AActor* const TargetActor = (*TargetHit).GetActor();
			if (!TargetActor || !CheckTagRequirementsForTarget(TargetActor))
			{
				TargetHit.RemoveCurrent();
			}
		}
	}

	Pins = OutHitResults.IsEmpty() && OutActors.IsEmpty() ? ETargetFinder_Pins::NoTargets : ETargetFinder_Pins::TargetsFound;
}

#if WITH_EDITOR
bool UTargetFinderBase::ImplementsGetWorld() const
{
	return true;
}

class UWorld* UTargetFinderBase::GetWorld() const
{
	return GetOuter() ? GetOuter()->GetWorld() : nullptr;
}
#endif