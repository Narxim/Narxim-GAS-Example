// Copyright 2021 Joseph "Narxim" Thigpen.
#pragma once

#include "CoreMinimal.h"
#include "Types.generated.h"

UENUM(BlueprintType)
enum class ECustomExecPin_Success2 : uint8
{
	Success,
	Failure
};

UENUM(BlueprintType)
enum class ECustomExecPin_Valid2 : uint8
{
	Valid,
	Invalid
};