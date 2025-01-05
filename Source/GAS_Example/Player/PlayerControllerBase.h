// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerBase.generated.h"

/**
 * Base Player Controller Class
 */
UCLASS()
class GAS_EXAMPLE_API APlayerControllerBase : public APlayerController, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};
