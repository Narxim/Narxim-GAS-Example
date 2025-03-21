// Copyright 2021 Joseph "Narxim" Thigpen.
#pragma once

#include "UObject/Interface.h"
#include "CRLActorInterface.generated.h"

class UCRLCombatManager;
class UCRLActorComponent;

UINTERFACE(Blueprintable)
class CUSTOMROGUELITETOOLS_API UCRLActorInterface : public UInterface
{
	GENERATED_BODY()
};

class CUSTOMROGUELITETOOLS_API ICRLActorInterface
{
	GENERATED_BODY()

public:
	// Direct access to the Component.
	UFUNCTION(BlueprintNativeEvent)
	UCRLActorComponent* GetCRLActorComponent() const;

	// Direct access to the Component.
	UFUNCTION(BlueprintNativeEvent)
	UCRLCombatManager* GetCRLCombatManager() const;
};