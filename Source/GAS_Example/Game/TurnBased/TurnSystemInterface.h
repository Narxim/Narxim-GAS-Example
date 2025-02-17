// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "UObject/Interface.h"
#include "TurnSystemInterface.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTurnChange, int32 NewTurn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnChangeAssignable, int32, NewTurn);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTurnSystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Manage the current turn and broadcast the `OnTurnChanged` event.
 */
class GAS_EXAMPLE_API ITurnSystemInterface
{
	GENERATED_BODY()

public:
	virtual FOnTurnChange& GetOnTurnChangeDelegate() = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Turn System")
	int32 GetCurrentTurn() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Turn System")
	void IncrementTurn();
};
