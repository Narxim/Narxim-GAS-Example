// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/GameplayTags/Public/NativeGameplayTags.h"

namespace TurnBasedNativeGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Effect_Type_TurnBased);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Effect_Modifier_InhibitionTurn);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Effect_Modifier_DurationTurn);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Effect_Modifier_MaxPeriodicApplications);
}
