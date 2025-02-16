// Copyright 2021 Joseph "Narxim" Thigpen.


#include "TurnExampleGameStateBase.h"

void ATurnExampleGameStateBase::IncrementTurn_Implementation()
{
	CurrentTurn++;
	OnTurnChange.Broadcast(CurrentTurn);
}
