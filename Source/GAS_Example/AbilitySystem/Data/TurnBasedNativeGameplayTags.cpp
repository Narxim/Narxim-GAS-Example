// Copyright 2021 Joseph "Narxim" Thigpen.
#include "TurnBasedNativeGameplayTags.h"

namespace TurnBasedNativeGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Effect_Type_TurnBased, "Effect.Type.TurnBased", "Turn based GE")
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Effect_Modifier_InhibitionTurn, "TurnBased.Modifier.InhibitionTurn", "Inhibition Turn Modifier")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Effect_Modifier_DurationTurn, "TurnBased.Modifier.DurationTurn", "DurationTurn Modifier")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Effect_Modifier_MaxPeriodicApplications, "TurnBased.Modifier.MaxPeriodicApplications", "MaxPeriodicApplications Modifier")
}