// Copyright 2021 Joseph "Narxim" Thigpen.


#include "Engine/CustomEngineSubsystem.h"
#include "AbilitySystemGlobals.h"

void UCustomEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UAbilitySystemGlobals::Get().InitGlobalData();	
}