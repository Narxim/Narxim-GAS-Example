// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "TurnSystemInterface.h"
#include "GameFramework/GameStateBase.h"
#include "TurnExampleGameStateBase.generated.h"

/**
 * Implements ITurnSystemInterface which manages the current turn and broadcasts the `OnTurnChanged` event.
 */
UCLASS()
class GAS_EXAMPLE_API ATurnExampleGameStateBase : public AGameStateBase, public ITurnSystemInterface
{
	GENERATED_BODY()

public:

	//~ Begin ITurnSystemInterface
	virtual FOnTurnChange& GetOnTurnChangeDelegate() override { return OnTurnChange; }

	UFUNCTION(BlueprintCallable)
	virtual int32 GetCurrentTurn_Implementation() const override { return CurrentTurn; }
	
	UFUNCTION(BlueprintCallable)
	virtual void IncrementTurn_Implementation() override;
	//~ End ITurnSystemInterface

private:
	// Non-dynamic delegate is not blueprint assignable. If you need Blueprint support, split the delegate into two parts:
	// Use this non-dynamic delegate for C++ binding with extra parameters.
	// Plus a separate dynamic delegate (DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam) for Blueprint events.
	FOnTurnChange OnTurnChange;

	int32 CurrentTurn = 0;		
};
