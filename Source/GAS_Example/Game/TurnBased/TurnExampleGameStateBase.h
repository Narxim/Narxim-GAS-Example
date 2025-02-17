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

	ATurnExampleGameStateBase();

	//~ Begin ITurnSystemInterface
	virtual FOnTurnChange& GetOnTurnChangeDelegate() override { return OnTurnChange; }

	virtual int32 GetCurrentTurn_Implementation() const override { return CurrentTurn; }
	
	virtual void IncrementTurn_Implementation() override;
	//~ End ITurnSystemInterface

	UFUNCTION(Server, Reliable)
	virtual void Server_IncrementTurn();

	UFUNCTION(BlueprintCallable, Category = "Turn System", meta = (bIgnoreSelf = "true", WorldContext = "WorldContextObject"))
	static ATurnExampleGameStateBase* GetTurnSystem(UObject* WorldContextObject);

protected:
	// Non-dynamic delegate is not blueprint assignable. If you need Blueprint support, split the delegate into two parts:
	// Use this non-dynamic delegate for C++ binding with extra parameters.
	
	FOnTurnChange OnTurnChange;
	
	UPROPERTY(BlueprintAssignable, Category = "Turn System")
	FOnTurnChangeAssignable OnTurnChangeDelegate;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentTurn)
	int32 CurrentTurn = 0;

	UFUNCTION()
	void OnRep_CurrentTurn() const
	{
		OnTurnChange.Broadcast(CurrentTurn);
		OnTurnChangeDelegate.Broadcast(CurrentTurn);
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
