// Copyright Adam Grodzki All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

AGRPRO_API DECLARE_LOG_CATEGORY_EXTERN(LogAGR, Display, All);

#define AGR_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogAGR, Verbosity, Format, ##__VA_ARGS__); \
}

#define AGR_WARN(Format, ...) \
{ \
	UE_LOG(LogAGR, Warning, Format, ##__VA_ARGS__); \
}

#define AGR_ERROR(Format, ...) \
{ \
	UE_LOG(LogAGR, Error, Format, ##__VA_ARGS__); \
}