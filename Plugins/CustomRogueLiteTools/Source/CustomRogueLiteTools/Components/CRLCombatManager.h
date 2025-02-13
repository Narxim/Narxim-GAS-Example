// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "Components/AGR_CombatManager.h"
#include "CRLCombatManager.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CUSTOMROGUELITETOOLS_API UCRLCombatManager : public UAGR_CombatManager
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCRLCombatManager();

protected:
public:
};
