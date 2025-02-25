// Copyright 2021 Joseph "Narxim" Thigpen.


#include "TurnExampleGameStateBase.h"

#include "GameplayEffect.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

ATurnExampleGameStateBase::ATurnExampleGameStateBase()
{
	SetReplicates(true);
}

void ATurnExampleGameStateBase::IncrementTurn()
{
	Server_IncrementTurn();
}

void ATurnExampleGameStateBase::Server_IncrementTurn_Implementation()
{
	CurrentTurn++;
	MARK_PROPERTY_DIRTY_FROM_NAME(ATurnExampleGameStateBase, CurrentTurn, this);
	OnRep_CurrentTurn();
}

void ATurnExampleGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params{};
	Params.bIsPushBased = true;
	Params.Condition = COND_None;

	DOREPLIFETIME_WITH_PARAMS_FAST(ATurnExampleGameStateBase, CurrentTurn, Params);
}
